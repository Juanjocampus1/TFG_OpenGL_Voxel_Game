#include "WorldGen.h"

WorldGen::WorldGen(int seed) {
    noise.SetSeed(seed);
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise.SetFrequency(0.02f); // Increased frequency for more irregular terrain
    noise.SetFractalOctaves(4); // Más detalle
    noise.SetFractalLacunarity(2.0f);
    noise.SetFractalGain(0.5f);
}

float WorldGen::getHeight(int x, int z) {
    float n = noise.GetNoise((float)x, (float)z); // -1 a 1
    return (n * 0.5f + 0.5f) * 20.0f + 2.0f; // Increased scale for more height variation
}

Biome WorldGen::getBiome(int x, int z) {
    float biomeNoise = noise.GetNoise((float)x * 0.002f, (float)z * 0.002f); // Escala más amplia para biomas
    if (biomeNoise < -0.3f) return PLAINS;
    else if (biomeNoise < 0.3f) return HILLS;
    else return DESERT;
}