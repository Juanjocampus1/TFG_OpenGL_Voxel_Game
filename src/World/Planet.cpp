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

void Planet::draw(class Shader& shader, class Camera& camera) {
    std::lock_guard<std::mutex> lock(chunksMutex);

    // Rebuild meshes on main thread for any dirty chunks (creates rawVertices & indices)
    bool anyRebuilt = false;
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
        }
    }

    if (anyRebuilt) batchDirty = true; // mark batch dirty so we upload new data

    // If batch not dirty, just draw existing buffers
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

    // Collect total required sizes
    size_t totalFloats =0;
    size_t totalIndices =0;
    for (auto& pair : chunks) {
        Chunk* c = pair.second;
        if (!c->hasMeshBuilt()) continue;
        totalFloats += c->getRawVertices().size() / sizeof(float);
        totalIndices += c->getIndices().size();
    }

    if (totalFloats ==0 || totalIndices ==0) return;

    // Prepare aggregated buffers
    std::vector<float> allVerts;
    std::vector<unsigned int> allIndices;
    allVerts.reserve(totalFloats);
    allIndices.reserve(totalIndices);

    for (auto& pair : chunks) {
        Chunk* c = pair.second;
        if (!c->hasMeshBuilt()) continue;
        const std::vector<char>& rv = c->getRawVertices();
        const std::vector<unsigned int>& idx = c->getIndices();
        if (rv.empty() || idx.empty()) continue;
        const float* fv = reinterpret_cast<const float*>(rv.data());
        size_t numFloats = rv.size() / sizeof(float);
        size_t numVerts = numFloats /5; //5 floats per vertex
        unsigned int vertexOffset = static_cast<unsigned int>(allVerts.size() /5);
        // transform vertex positions to world space and append
        for (size_t v =0; v < numVerts; ++v) {
            float lx = fv[v *5 +0];
            float ly = fv[v *5 +1];
            float lz = fv[v *5 +2];
            float u = fv[v *5 +3];
            float vv = fv[v *5 +4];
            float wx = lx + c->position.x * (float)Chunk::SIZE;
            float wy = ly + c->position.y * (float)Chunk::SIZE;
            float wz = lz + c->position.z * (float)Chunk::SIZE;
            allVerts.push_back(wx);
            allVerts.push_back(wy);
            allVerts.push_back(wz);
            allVerts.push_back(u);
            allVerts.push_back(vv);
        }
        for (unsigned int i : idx) allIndices.push_back(i + vertexOffset);
    }

    // Create persistent buffers if needed
    if (!batchVao) batchVao = new VAO();
    if (!batchVbo || batchVertCapacity < allVerts.size()) {
        if (batchVbo) { batchVbo->deleteBuffer(); delete batchVbo; }
        batchVbo = new VBO(nullptr, static_cast<GLsizeiptr>(allVerts.size() * sizeof(float)));
        batchVertCapacity = allVerts.size();
    }
    if (!batchEbo || batchIndexCapacity < allIndices.size()) {
        if (batchEbo) { batchEbo->deleteEBO(); delete batchEbo; }
        batchEbo = new EBO(nullptr, static_cast<GLsizeiptr>(allIndices.size() * sizeof(unsigned int)));
        batchIndexCapacity = allIndices.size();
    }

    // Upload data using glMapBufferRange for better performance
    batchVao->bind();
    batchVbo->bind();
    GLsizeiptr vertSizeBytes = static_cast<GLsizeiptr>(allVerts.size() * sizeof(float));
    // allocate if needed
    if (batchVertCapacity * sizeof(float) < vertSizeBytes) {
        glBufferData(GL_ARRAY_BUFFER, vertSizeBytes, nullptr, GL_DYNAMIC_DRAW);
        batchVertCapacity = allVerts.size();
    }
    void* dst = glMapBufferRange(GL_ARRAY_BUFFER,0, vertSizeBytes, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
    if (dst) {
        memcpy(dst, allVerts.data(), vertSizeBytes);
        glUnmapBuffer(GL_ARRAY_BUFFER);
    } else {
        // fallback
        glBufferSubData(GL_ARRAY_BUFFER,0, vertSizeBytes, allVerts.data());
    }

    batchEbo->bind();
    GLsizeiptr idxSizeBytes = static_cast<GLsizeiptr>(allIndices.size() * sizeof(unsigned int));
    if (batchIndexCapacity * sizeof(unsigned int) < idxSizeBytes) {
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, idxSizeBytes, nullptr, GL_DYNAMIC_DRAW);
        batchIndexCapacity = allIndices.size();
    }
    void* idst = glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER,0, idxSizeBytes, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
    if (idst) {
        memcpy(idst, allIndices.data(), idxSizeBytes);
        glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
    } else {
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,0, idxSizeBytes, allIndices.data());
    }
    batchVao->linkVBO(*batchVbo,0,3, GL_FLOAT,5 * sizeof(float), (void*)0);
    batchVao->linkVBO(*batchVbo,1,2, GL_FLOAT,5 * sizeof(float), (void*)(3 * sizeof(float)));
    batchVao->unbind();

    // Draw
    shader.use();
    shader.setMat4("model", glm::mat4(1.0f));
    shader.setMat4("view", camera.GetViewMatrix());
    shader.setMat4("projection", camera.GetProjectionMatrix());

    batchVao->bind();
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(allIndices.size()), GL_UNSIGNED_INT,0);
    batchVao->unbind();
}