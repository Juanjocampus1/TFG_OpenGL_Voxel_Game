#ifndef BLOCKS_H
#define BLOCKS_H

#include "Block.h"
#include <string>

class Blocks {
public:
    static std::string getName(BlockType type);
    static bool isSolid(BlockType type);
};

#endif