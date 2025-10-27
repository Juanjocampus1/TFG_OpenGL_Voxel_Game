#include "Planet.h"
#include "../Renderer/Shader.h"
#include "../Renderer/Camera.h"
#include <algorithm>
#include <thread>
#include <chrono>

Planet::Planet(int renderDist) : renderDistance(renderDist), cameraPos(0.0f), worldGen(12345) {
    // start worker threads
    unsigned workerCount = std::max(1u, std::thread::hardware_concurrency() -1);
    for (unsigned i=0;i< workerCount;++i) {
        workers.emplace_back(&Planet::workerLoop, this);
    }
}

Planet::~Planet() {
    stopWorkers = true;
    taskCV.notify_all();
    for (auto &w : workers) if (w.joinable()) w.join();
    // existing cleanup
    for (auto& task : generationTasks) { if (task.valid()) task.wait(); }
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
    if (it != chunks.end()) {
        return it->second;
    }
    return nullptr;
}

void Planet::generateChunk(int x, int y, int z) {
    // push task to queue for workers
    {
        std::lock_guard<std::mutex> lock(taskMutex);
        taskQueue.emplace(x,y,z);
    }
    taskCV.notify_one();
}

void Planet::workerLoop() {
    while (!stopWorkers) {
        std::tuple<int,int,int> job;
        {
            std::unique_lock<std::mutex> lock(taskMutex);
            taskCV.wait(lock, [this]{ return !taskQueue.empty() || stopWorkers.load(); });
            if (stopWorkers && taskQueue.empty()) return;
            job = taskQueue.front(); taskQueue.pop();
        }
        int x,y,z; std::tie(x,y,z)=job;
        // Generate chunk data off-thread (only block data, no GL calls)
        Chunk* c = new Chunk(x,y,z,nullptr);
        c->setWorldGen(&worldGen);
        c->generate();

        // Insert chunk into shared map on worker thread under lock (no GL calls here)
        {
            std::lock_guard<std::mutex> lock(chunksMutex);
            auto key = std::make_tuple(x,y,z);
            if (chunks.find(key) == chunks.end()) {
                chunks[key] = c;
                c->meshDirty = true;
                // mark neighbors as dirty so their meshes will be rebuilt on main thread
                std::vector<std::tuple<int,int,int>> neighbors = { {x+1,y,z}, {x-1,y,z}, {x,y,z+1}, {x,y,z-1} };
                for (auto& n : neighbors) {
                    auto it = chunks.find(n);
                    if (it != chunks.end()) it->second->meshDirty = true;
                }
            } else {
                // another thread already created it; discard
                delete c;
            }
        }
    }
}

void Planet::loadChunksAround() {
    int chunkX = static_cast<int>(std::floor(cameraPos.x / Chunk::SIZE));
    int chunkZ = static_cast<int>(std::floor(cameraPos.z / Chunk::SIZE));

    int minX = chunkX - renderDistance;
    int maxX = chunkX + renderDistance;
    int minZ = chunkZ - renderDistance;
    int maxZ = chunkZ + renderDistance;

    // First pass: determine which chunk coordinates are needed (including vertical based on height)
    std::vector<std::tuple<int,int,int>> toCreate;
    toCreate.reserve((maxX - minX +1) * (maxZ - minZ +1) *2);

    {
        std::lock_guard<std::mutex> lock(chunksMutex);
        for (int x = minX; x <= maxX; ++x) {
            for (int z = minZ; z <= maxZ; ++z) {
                // Compute the max and min terrain heights across this chunk area to avoid holes
                float maxH = -1e9f;
                float minH =1e9f;
                for (int lx =0; lx < Chunk::SIZE; ++lx) {
                    for (int lz =0; lz < Chunk::SIZE; ++lz) {
                        int worldX = x * Chunk::SIZE + lx;
                        int worldZ = z * Chunk::SIZE + lz;
                        float h = worldGen.getHeight(worldX, worldZ);
                        if (h > maxH) maxH = h;
                        if (h < minH) minH = h;
                    }
                }

                int maxChunkY = static_cast<int>(std::floor(maxH / Chunk::SIZE));
                int minChunkY = static_cast<int>(std::floor(minH / Chunk::SIZE));

                // Cap vertical range to avoid generating extremely tall stacks
                const int MAX_VERTICAL_CHUNKS =6; // allow up to this many vertical chunks
                if (maxChunkY - minChunkY +1 > MAX_VERTICAL_CHUNKS) {
                    // center the range around0..MAX_VERTICAL_CHUNKS-1
                    int center = std::max(0, (minChunkY + maxChunkY) /2);
                    minChunkY = center - (MAX_VERTICAL_CHUNKS/2);
                    maxChunkY = minChunkY + MAX_VERTICAL_CHUNKS -1;
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
    unsigned maxTasks = std::max(1u, std::thread::hardware_concurrency() -1);
    for (auto &key : toCreate) {
        // Clean up finished tasks
        generationTasks.erase(std::remove_if(generationTasks.begin(), generationTasks.end(),
        [](std::future<void>& f) { return f.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready; }), generationTasks.end());

        while (generationTasks.size() >= maxTasks) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            generationTasks.erase(std::remove_if(generationTasks.begin(), generationTasks.end(),
            [](std::future<void>& f) { return f.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready; }), generationTasks.end());
        }

        int x, y, z;
        std::tie(x,y,z) = key;
        generationTasks.push_back(std::async(std::launch::async, &Planet::generateChunk, this, x, y, z));
        loaded++;
    }
}

void Planet::unloadChunks() {
    std::lock_guard<std::mutex> lock(chunksMutex);
    glm::vec3 cameraChunkPos = glm::vec3(cameraPos.x / (float)Chunk::SIZE, 0.0f, cameraPos.z / (float)Chunk::SIZE);
    for (auto it = chunks.begin(); it != chunks.end(); ) {
        int x, y, z;
        std::tie(x, y, z) = it->first;
        glm::vec3 chunkPos(x, y, z);
        float dist = glm::distance(cameraChunkPos, chunkPos);
        if (dist > renderDistance + 1) { // +1 for buffer
            delete it->second;
            it = chunks.erase(it);
            unloaded++;
        } else {
            ++it;
        }
    }
}

int Planet::allocateSlot(const std::tuple<int,int,int>& key, size_t vertBytes, size_t idxBytes) {
    // allocate contiguous space at the end
    SlotInfo info;
    info.vertOffsetBytes = totalVertBytes;
    info.vertSizeBytes = vertBytes;
    info.idxOffsetBytes = totalIndexBytes;
    info.idxSizeBytes = idxBytes;
    info.vertexBase = (totalVertBytes / sizeof(float)) /5; //5 floats per vertex

    slotMap[key] = info;

    totalVertBytes += vertBytes;
    totalIndexBytes += idxBytes;

    // return dummy slot id as not used elsewhere
    return 0;
}

void Planet::freeSlot(const std::tuple<int,int,int>& key) {
    auto it = slotMap.find(key);
    if (it != slotMap.end()) {
        // mark removed (we don't compact buffers to keep implementation simple)
        slotMap.erase(it);
    }
}

void Planet::draw(class Shader& shader, class Camera& camera) {
    std::lock_guard<std::mutex> lock(chunksMutex);

    // Rebuild meshes on main thread for any dirty chunks (creates rawVertices & indices)
    bool anyRebuilt = false;
    std::vector<std::tuple<int,int,int>> rebuiltKeys;
    for (auto& pair : chunks) {
        Chunk* c = pair.second;
        if (c->meshDirty) {
            int delay = c->getDirtyDelay();
            if (delay >0) {
                // decrement debounce counter and skip rebuild this frame
                c->setDirtyDelay(delay -1);
                continue;
            }
            c->rebuildMesh(chunks);
            c->meshDirty = false;
            c->meshBuilt = true;
            anyRebuilt = true;
            rebuiltKeys.push_back(pair.first);
        }
    }

    if (anyRebuilt) batchDirty = true; // mark batch dirty so we upload new data

    // If no rebuilds and batch is clean, draw existing buffers
    if (!batchDirty && batchVao) {
        shader.use();
        shader.setMat4("model", glm::mat4(1.0f));
        shader.setMat4("view", camera.GetViewMatrix());
        shader.setMat4("projection", camera.GetProjectionMatrix());
        batchVao->bind();
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(batchIndexCount), GL_UNSIGNED_INT,0);
        batchVao->unbind();
        return;
    }

    // Build list of chunks that have meshes
    std::vector<std::pair<std::tuple<int,int,int>, Chunk*>> chunkList;
    chunkList.reserve(chunks.size());
    for (auto& pair : chunks) {
        if (!pair.second->hasMeshBuilt()) continue;
        chunkList.emplace_back(pair.first, pair.second);
    }

    // For each chunk, ensure it has a slot and upload only that slot if needed
    // First, make sure we have batch buffers large enough
    if (!batchVao) batchVao = new VAO();
    // Ensure batch VBO/EBO exist with at least current totals
    if (!batchVbo) {
        batchVbo = new VBO(nullptr, static_cast<GLsizeiptr>(totalVertBytes));
        batchVertCapacity = totalVertBytes;
    }
    if (!batchEbo) {
        batchEbo = new EBO(nullptr, static_cast<GLsizeiptr>(totalIndexBytes));
        batchIndexCapacity = totalIndexBytes;
    }

    // Allocate slots as needed (this updates totalVertBytes/totalIndexBytes)
    for (size_t ci =0; ci < chunkList.size(); ++ci) {
        const auto& key = chunkList[ci].first;
        Chunk* c = chunkList[ci].second;
        const std::vector<char>& rv = c->getRawVertices();
        const std::vector<unsigned int>& idx = c->getIndices();
        if (rv.empty() || idx.empty()) continue;
        size_t vertBytes = rv.size();
        size_t idxBytes = idx.size() * sizeof(unsigned int);
        auto it = slotMap.find(key);
        if (it == slotMap.end()) {
            allocateSlot(key, vertBytes, idxBytes);
        } else {
            // if existing slot too small, free and reallocate
            SlotInfo existing = it->second;
            if (existing.vertSizeBytes < vertBytes || existing.idxSizeBytes < idxBytes) {
                freeSlot(key);
                allocateSlot(key, vertBytes, idxBytes);
            }
        }
    }

    // If totals changed (due to allocations), reallocate GPU buffers to new sizes
    if (!batchVbo) batchVbo = new VBO(nullptr, static_cast<GLsizeiptr>(totalVertBytes));
    if (!batchEbo) batchEbo = new EBO(nullptr, static_cast<GLsizeiptr>(totalIndexBytes));
    if (totalVertBytes > batchVertCapacity) {
        batchVbo->bind();
        glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)totalVertBytes, nullptr, GL_DYNAMIC_DRAW);
        batchVertCapacity = totalVertBytes;
    }
    if (totalIndexBytes > batchIndexCapacity) {
        batchEbo->bind();
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)totalIndexBytes, nullptr, GL_DYNAMIC_DRAW);
        batchIndexCapacity = totalIndexBytes;
    }

    // Now upload per-slot data
    batchVbo->bind();
    batchEbo->bind();
    for (size_t ci =0; ci < chunkList.size(); ++ci) {
        const auto& key = chunkList[ci].first;
        Chunk* c = chunkList[ci].second;
        const std::vector<char>& rv = c->getRawVertices();
        const std::vector<unsigned int>& idx = c->getIndices();
        if (rv.empty() || idx.empty()) continue;
        auto it = slotMap.find(key);
        if (it == slotMap.end()) continue; // should not happen
        SlotInfo info = it->second;
        // upload vertex block
        if (info.vertSizeBytes >0) {
            glBufferSubData(GL_ARRAY_BUFFER, (GLintptr)info.vertOffsetBytes, (GLsizeiptr)info.vertSizeBytes, rv.data());
        }
        // upload adjusted indices
        if (info.idxSizeBytes >0) {
            size_t idxCount = idx.size();
            std::vector<unsigned int> adj; adj.reserve(idxCount);
            for (size_t k =0; k < idxCount; ++k) adj.push_back(idx[k] + static_cast<unsigned int>(info.vertexBase));
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, (GLintptr)info.idxOffsetBytes, (GLsizeiptr)info.idxSizeBytes, adj.data());
        }
    }

    // Link attributes once
    batchVao->bind();
    batchVao->linkVBO(*batchVbo,0,3, GL_FLOAT,5 * sizeof(float), (void*)0);
    batchVao->linkVBO(*batchVbo,1,2, GL_FLOAT,5 * sizeof(float), (void*)(3 * sizeof(float)));
    batchEbo->bind();
    batchVao->unbind();

    // Update batch index count and draw
    batchIndexCount = totalIndexBytes / sizeof(unsigned int);

    shader.use();
    shader.setMat4("model", glm::mat4(1.0f));
    shader.setMat4("view", camera.GetViewMatrix());
    shader.setMat4("projection", camera.GetProjectionMatrix());

    batchVao->bind();
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(batchIndexCount), GL_UNSIGNED_INT,0);
    batchVao->unbind();

    batchDirty = false;
}