#include "WorldGen.h"
#include <cmath>

WorldGen::WorldGen(int seed) : cacheSeed(seed) {
    noise.SetSeed(seed);
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    noise.SetFrequency(0.01f); // Más bajo para terrain más suave
    noise.SetFractalType(FastNoiseLite::FractalType_FBm);
    noise.SetFractalOctaves(4);
    noise.SetFractalLacunarity(2.0f);
    noise.SetFractalGain(0.5f);

    // Initialize cache
    heightCache.reserve(CACHE_SIZE);
}

float WorldGen::getHeight(int x, int z) {
    // Check cache first
    uint64_t key = (static_cast<uint64_t>(static_cast<uint32_t>(x)) << 32) | 
                    static_cast<uint64_t>(static_cast<uint32_t>(z));

    {
        std::lock_guard<std::mutex> lock(cacheMutex);
        auto it = heightCache.find(key);
        if (it != heightCache.end()) {
            return it->second;
        }
    }

    // Calculate height - usar coordenadas absolutas consistentemente
    float n = noise.GetNoise(static_cast<float>(x), static_cast<float>(z));
    
    // Normalizar noise de [-1, 1] a [0, 1]
    float normalized = (n + 1.0f) * 0.5f;
    
    // Aplicar curva para mejor distribución
    normalized = std::pow(normalized, 1.5f);
    
    // Calcular altura final
    float height = normalized * 30.0f + 5.0f; // Rango 5-35 blocks

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