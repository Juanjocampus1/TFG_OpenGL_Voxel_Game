#include "Planet.h"
#include "../Renderer/Shader.h"
#include "../Renderer/Camera.h"
#include <algorithm>
#include <thread>
#include <chrono>

Planet::Planet(int renderDist) : renderDistance(renderDist), cameraPos(0.0f), worldGen(12345),
batchVao(nullptr), batchVbo(nullptr), batchEbo(nullptr),
batchDirty(false), batchVertCapacity(0), batchIndexCapacity(0),
totalVertBytes(0), totalIndexBytes(0), batchIndexCount(0) {

    // Start worker threads
    unsigned workerCount = std::max(1u, std::thread::hardware_concurrency() - 1);
    for (unsigned i = 0; i < workerCount; ++i) {
        workers.emplace_back(&Planet::workerLoop, this);
    }
}

Planet::~Planet() {
    stopWorkers = true;
    taskCV.notify_all();
    for (auto& w : workers) {
        if (w.joinable()) w.join();
    }

    for (auto& task : generationTasks) {
        if (task.valid()) task.wait();
    }

    std::lock_guard<std::mutex> lock(chunksMutex);
    for (auto& pair : chunks) delete pair.second;
    chunks.clear();

    if (batchVbo) { batchVbo->deleteBuffer(); delete batchVbo; }
    if (batchEbo) { batchEbo->deleteEBO(); delete batchEbo; }
    if (batchVao) delete batchVao;
}

void Planet::setCameraPos(const glm::vec3& pos) {
    cameraPos = pos;
}

void Planet::updateChunks() {
    loaded = 0;
    unloaded = 0;
    loadChunksAround();
    unloadChunks();
}

Chunk* Planet::getChunk(int x, int y, int z) {
    std::lock_guard<std::mutex> lock(chunksMutex);
    auto key = std::make_tuple(x, y, z);
    auto it = chunks.find(key);
    return (it != chunks.end()) ? it->second : nullptr;
}

void Planet::generateChunk(int x, int y, int z) {
    std::lock_guard<std::mutex> lock(taskMutex);
    taskQueue.emplace(x, y, z);
    taskCV.notify_one();
}

void Planet::workerLoop() {
    while (!stopWorkers) {
        std::tuple<int, int, int> job;
        {
            std::unique_lock<std::mutex> lock(taskMutex);
            taskCV.wait(lock, [this] { return !taskQueue.empty() || stopWorkers.load(); });
            if (stopWorkers && taskQueue.empty()) return;
            job = taskQueue.front();
            taskQueue.pop();
        }

        int x, y, z;
        std::tie(x, y, z) = job;

        // Generate chunk data off-thread (no GL calls)
        Chunk* c = new Chunk(x, y, z, nullptr);
        c->setWorldGen(&worldGen);
        c->generate();

        // Insert chunk into shared map
        {
            std::lock_guard<std::mutex> lock(chunksMutex);
            auto key = std::make_tuple(x, y, z);
            if (chunks.find(key) == chunks.end()) {
                chunks[key] = c;
                c->meshDirty = true;

                // Mark neighbors as dirty
                std::vector<std::tuple<int, int, int>> neighbors = {
                    {x + 1,y,z}, {x - 1,y,z}, {x,y + 1,z}, {x,y - 1,z}, {x,y,z + 1}, {x,y,z - 1}
                };
                for (auto& n : neighbors) {
                    auto it = chunks.find(n);
                    if (it != chunks.end()) {
                        it->second->meshDirty = true;
                    }
                }
            }
            else {
                delete c; // Already created by another thread
            }
        }
    }
}

void Planet::loadChunksAround() {
    int chunkX = static_cast<int>(std::floor(cameraPos.x / Chunk::SIZE));
    int chunkY = static_cast<int>(std::floor(cameraPos.y / Chunk::SIZE));
    int chunkZ = static_cast<int>(std::floor(cameraPos.z / Chunk::SIZE));

    std::vector<std::tuple<int, int, int>> toCreate;
    toCreate.reserve((renderDistance * 2 + 1) * (renderDistance * 2 + 1) * 6);

    {
        std::lock_guard<std::mutex> lock(chunksMutex);

        for (int x = chunkX - renderDistance; x <= chunkX + renderDistance; ++x) {
            for (int z = chunkZ - renderDistance; z <= chunkZ + renderDistance; ++z) {
                // Only load chunks within circular distance
                float dx = (x - chunkX);
                float dz = (z - chunkZ);
                if (dx * dx + dz * dz > renderDistance * renderDistance) continue;

                // Calculate height range for this column
                float maxH = -1e9f;
                float minH = 1e9f;

                // Sample fewer points for height check
                for (int lx = 0; lx < Chunk::SIZE; lx += 4) {
                    for (int lz = 0; lz < Chunk::SIZE; lz += 4) {
                        int worldX = x * Chunk::SIZE + lx;
                        int worldZ = z * Chunk::SIZE + lz;
                        float h = worldGen.getHeight(worldX, worldZ);
                        maxH = std::max(maxH, h);
                        minH = std::min(minH, h);
                    }
                }

                int maxChunkY = static_cast<int>(std::floor(maxH / Chunk::SIZE));
                int minChunkY = static_cast<int>(std::floor(minH / Chunk::SIZE));

                // Cap vertical range
                const int MAX_VERTICAL_CHUNKS = 4;
                if (maxChunkY - minChunkY + 1 > MAX_VERTICAL_CHUNKS) {
                    int center = (minChunkY + maxChunkY) / 2;
                    minChunkY = center - (MAX_VERTICAL_CHUNKS / 2);
                    maxChunkY = minChunkY + MAX_VERTICAL_CHUNKS - 1;
                }

                for (int y = minChunkY; y <= maxChunkY; ++y) {
                    auto key = std::make_tuple(x, y, z);
                    if (chunks.find(key) == chunks.end()) {
                        toCreate.push_back(key);
                    }
                }
            }
        }
    }

    // Launch generation tasks (throttled)
    const unsigned maxTasks = std::max(2u, std::thread::hardware_concurrency());
    for (auto& key : toCreate) {
        // Clean up finished tasks
        generationTasks.erase(
            std::remove_if(generationTasks.begin(), generationTasks.end(),
                [](std::future<void>& f) {
                    return f.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready;
                }),
            generationTasks.end()
        );

        while (generationTasks.size() >= maxTasks) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            generationTasks.erase(
                std::remove_if(generationTasks.begin(), generationTasks.end(),
                    [](std::future<void>& f) {
                        return f.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready;
                    }),
                generationTasks.end()
            );
        }

        int x, y, z;
        std::tie(x, y, z) = key;
        generationTasks.push_back(
            std::async(std::launch::async, &Planet::generateChunk, this, x, y, z)
        );
        loaded++;
    }
}

void Planet::unloadChunks() {
    std::lock_guard<std::mutex> lock(chunksMutex);
    glm::vec3 cameraChunkPos(
        cameraPos.x / static_cast<float>(Chunk::SIZE),
        cameraPos.y / static_cast<float>(Chunk::SIZE),
        cameraPos.z / static_cast<float>(Chunk::SIZE)
    );

    for (auto it = chunks.begin(); it != chunks.end(); ) {
        int x, y, z;
        std::tie(x, y, z) = it->first;

        float dx = x - cameraChunkPos.x;
        float dy = y - cameraChunkPos.y;
        float dz = z - cameraChunkPos.z;
        float dist = std::sqrt(dx * dx + dy * dy + dz * dz);

        if (dist > renderDistance + 2) {
            // Free slot if exists
            freeSlot(it->first);
            delete it->second;
            it = chunks.erase(it);
            unloaded++;
            batchDirty = true;
        }
        else {
            ++it;
        }
    }
}

int Planet::allocateSlot(const std::tuple<int, int, int>& key, size_t vertBytes, size_t idxBytes) {
    SlotInfo info;
    info.vertOffsetBytes = totalVertBytes;
    info.vertSizeBytes = vertBytes;
    info.idxOffsetBytes = totalIndexBytes;
    info.idxSizeBytes = idxBytes;
    info.vertexBase = totalVertBytes / (sizeof(float) * 5); // 5 floats per vertex

    slotMap[key] = info;

    totalVertBytes += vertBytes;
    totalIndexBytes += idxBytes;

    return 0;
}

void Planet::freeSlot(const std::tuple<int, int, int>& key) {
    slotMap.erase(key);
}

void Planet::draw(Shader& shader, Camera& camera) {
    std::lock_guard<std::mutex> lock(chunksMutex);

    // Rebuild meshes on main thread for dirty chunks
    const int MAX_REBUILDS_PER_FRAME = 4; // Limit rebuilds per frame
    int rebuiltCount = 0;

    for (auto& pair : chunks) {
        Chunk* c = pair.second;
        if (c->meshDirty && rebuiltCount < MAX_REBUILDS_PER_FRAME) {
            int delay = c->getDirtyDelay();
            if (delay > 0) {
                c->setDirtyDelay(delay - 1);
                continue;
            }

            c->rebuildMesh(chunks);
            c->meshDirty = false;
            c->meshBuilt = true;
            batchDirty = true;
            rebuiltCount++;
        }
    }

    // If batch clean and buffers exist, just draw
    if (!batchDirty && batchVao && batchIndexCount > 0) {
        shader.use();
        shader.setMat4("model", glm::mat4(1.0f)); // Identity - verts already in world space
        shader.setMat4("view", camera.GetViewMatrix());
        shader.setMat4("projection", camera.GetProjectionMatrix());

        batchVao->bind();
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(batchIndexCount), GL_UNSIGNED_INT, 0);
        batchVao->unbind();
        return;
    }

    // Rebuild batch buffers
    std::vector<std::pair<std::tuple<int, int, int>, Chunk*>> chunkList;
    chunkList.reserve(chunks.size());

    for (auto& pair : chunks) {
        if (pair.second->hasMeshBuilt() &&
            !pair.second->getRawVertices().empty() &&
            !pair.second->getIndices().empty()) {
            chunkList.emplace_back(pair.first, pair.second);
        }
    }

    if (chunkList.empty()) return;

    // Calculate total size needed
    size_t newTotalVertBytes = 0;
    size_t newTotalIndexBytes = 0;
    std::map<std::tuple<int, int, int>, SlotInfo> newSlotMap;

    for (size_t i = 0; i < chunkList.size(); ++i) {
        const auto& key = chunkList[i].first;
        Chunk* chunk = chunkList[i].second;
        const auto& rv = chunk->getRawVertices();
        const auto& idx = chunk->getIndices();

        SlotInfo info;
        info.vertOffsetBytes = newTotalVertBytes;
        info.vertSizeBytes = rv.size();
        info.idxOffsetBytes = newTotalIndexBytes;
        info.idxSizeBytes = idx.size() * sizeof(unsigned int);
        info.vertexBase = newTotalVertBytes / (sizeof(float) * 5);

        newSlotMap[key] = info;
        newTotalVertBytes += info.vertSizeBytes;
        newTotalIndexBytes += info.idxSizeBytes;
    }

    // Reallocate GPU buffers if needed
    if (!batchVao) batchVao = new VAO();

    bool needsRealloc = false;
    if (!batchVbo || newTotalVertBytes > batchVertCapacity) {
        if (batchVbo) { batchVbo->deleteBuffer(); delete batchVbo; }
        batchVbo = new VBO(nullptr, static_cast<GLsizeiptr>(newTotalVertBytes));
        batchVertCapacity = newTotalVertBytes;
        needsRealloc = true;
    }

    if (!batchEbo || newTotalIndexBytes > batchIndexCapacity) {
        if (batchEbo) { batchEbo->deleteEBO(); delete batchEbo; }
        batchEbo = new EBO(nullptr, static_cast<GLsizeiptr>(newTotalIndexBytes));
        batchIndexCapacity = newTotalIndexBytes;
        needsRealloc = true;
    }

    // Upload data
    batchVbo->bind();
    batchEbo->bind();

    for (size_t i = 0; i < chunkList.size(); ++i) {
        const auto& key = chunkList[i].first;
        Chunk* chunk = chunkList[i].second;
        const auto& rv = chunk->getRawVertices();
        const auto& idx = chunk->getIndices();
        const SlotInfo& info = newSlotMap[key];

        // Upload vertices
        if (info.vertSizeBytes > 0) {
            glBufferSubData(GL_ARRAY_BUFFER,
                static_cast<GLintptr>(info.vertOffsetBytes),
                static_cast<GLsizeiptr>(info.vertSizeBytes),
                rv.data());
        }

        // Upload adjusted indices
        if (info.idxSizeBytes > 0) {
            std::vector<unsigned int> adjustedIndices;
            adjustedIndices.reserve(idx.size());
            for (unsigned int index : idx) {
                adjustedIndices.push_back(index + static_cast<unsigned int>(info.vertexBase));
            }
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,
                static_cast<GLintptr>(info.idxOffsetBytes),
                static_cast<GLsizeiptr>(info.idxSizeBytes),
                adjustedIndices.data());
        }
    }

    // Setup VAO attributes
    batchVao->bind();
    batchVao->linkVBO(*batchVbo, 0, 3, GL_FLOAT, 5 * sizeof(float), (void*)0);
    batchVao->linkVBO(*batchVbo, 1, 2, GL_FLOAT, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    batchEbo->bind();
    batchVao->unbind();

    // Update state
    slotMap = std::move(newSlotMap);
    totalVertBytes = newTotalVertBytes;
    totalIndexBytes = newTotalIndexBytes;
    batchIndexCount = totalIndexBytes / sizeof(unsigned int);
    batchDirty = false;

    // Draw
    shader.use();
    shader.setMat4("model", glm::mat4(1.0f));
    shader.setMat4("view", camera.GetViewMatrix());
    shader.setMat4("projection", camera.GetProjectionMatrix());

    batchVao->bind();
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(batchIndexCount), GL_UNSIGNED_INT, 0);
    batchVao->unbind();
}