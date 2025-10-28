#pragma once
#include "../FastNoiseLite.h"
#include <unordered_map>
#include <mutex>

enum Biome {
    PLAINS,
    HILLS,
    DESERT
};

class WorldGen {
private:
    FastNoiseLite noise;
    int cacheSeed;

    // Height cache for performance
    static const size_t CACHE_SIZE = 16384; // Adjust based on memory constraints
    std::unordered_map<uint64_t, float> heightCache;
    std::mutex cacheMutex;

public:
    WorldGen(int seed);
    float getHeight(int x, int z);
    Biome getBiome(int x, int z);
};