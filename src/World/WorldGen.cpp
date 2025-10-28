#include "WorldGen.h"
#include <cmath>

WorldGen::WorldGen(int seed) : cacheSeed(seed) {
    noise.SetSeed(seed);
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise.SetFrequency(0.02f);
    noise.SetFractalOctaves(4);
    noise.SetFractalLacunarity(2.0f);
    noise.SetFractalGain(0.5f);

    // Initialize cache
    heightCache.reserve(CACHE_SIZE);
}

float WorldGen::getHeight(int x, int z) {
    // Check cache first
    uint64_t key = (static_cast<uint64_t>(x) << 32) | static_cast<uint64_t>(z);

    {
        std::lock_guard<std::mutex> lock(cacheMutex);
        auto it = heightCache.find(key);
        if (it != heightCache.end()) {
            return it->second;
        }
    }

    // Calculate height
    float n = noise.GetNoise(static_cast<float>(x), static_cast<float>(z));
    float height = (n * 0.5f + 0.5f) * 20.0f + 2.0f;

    // Store in cache (with size limit)
    {
        std::lock_guard<std::mutex> lock(cacheMutex);
        if (heightCache.size() >= CACHE_SIZE) {
            // Simple eviction: clear half the cache when full
            auto it = heightCache.begin();
            std::advance(it, CACHE_SIZE / 2);
            heightCache.erase(heightCache.begin(), it);
        }
        heightCache[key] = height;
    }

    return height;
}

Biome WorldGen::getBiome(int x, int z) {
    float biomeNoise = noise.GetNoise(static_cast<float>(x) * 0.002f,
        static_cast<float>(z) * 0.002f);
    if (biomeNoise < -0.3f) return PLAINS;
    else if (biomeNoise < 0.3f) return HILLS;
    else return DESERT;
}