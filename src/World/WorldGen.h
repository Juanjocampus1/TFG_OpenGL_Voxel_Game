#ifndef WORLDGEN_H
#define WORLDGEN_H

#include "../FastNoiseLite.h"

enum Biome { PLAINS, HILLS, DESERT };

class WorldGen {
public:
    WorldGen(int seed);
    float getHeight(int x, int z);
    Biome getBiome(int x, int z);
private:
    FastNoiseLite noise;
};

#endif