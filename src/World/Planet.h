#pragma once

#include <map>
#include <tuple>
#include <vector>
#include <thread>
#include <mutex>
#include <future>
#include <glm/glm.hpp>
#include "Chunk.h"
#include "WorldGen.h"
#include <queue>
#include <condition_variable>
#include <atomic>

class Planet {
private:
    std::map<std::tuple<int, int, int>, Chunk*> chunks;
    std::mutex chunksMutex;
    int renderDistance;
    glm::vec3 cameraPos;
    std::vector<std::future<void>> generationTasks;
    WorldGen worldGen;

    // Batch rendering resources
    VAO* batchVao = nullptr;
    VBO* batchVbo = nullptr;
    EBO* batchEbo = nullptr;
    size_t batchVertCapacity = 0;
    size_t batchIndexCapacity = 0;
    size_t batchVertCount = 0; // number of floats uploaded
    size_t batchIndexCount = 0; // number of indices uploaded
    bool batchDirty = true;

    // Thread pool for chunk generation
    std::vector<std::thread> workers;
    std::queue<std::tuple<int, int, int>> taskQueue;
    std::mutex taskMutex;
    std::condition_variable taskCV;
    std::vector<std::tuple<int, int, int>> pendingChunks; // tasks completed by workers, need main-thread processing
    std::mutex pendingMutex;
    std::atomic<bool> stopWorkers{ false };

public:
    Planet(int renderDist);
    ~Planet();
    void setCameraPos(const glm::vec3& pos);
    void updateChunks();
    Chunk* getChunk(int x, int, int);
    void draw(class Shader& shader, class Camera& camera);
    int loaded, unloaded;

private:
    void generateChunk(int x, int y, int z);
    void loadChunksAround();
    void unloadChunks();
    void workerLoop();
};