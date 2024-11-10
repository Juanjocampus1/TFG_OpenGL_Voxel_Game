#pragma once

#include <vector>
#include "Block.h"
#include <glm/glm.hpp>

namespace Blocks {
    const std::vector<Block> blocks{
        Block(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f)), // Air block
        Block(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f)), // Dirt block
        Block(glm::vec2(1.0f, 1.0f), glm::vec2(2.0f, 2.0f),  // Grass block
              glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f),
              glm::vec2(1.0f, 0.0f), glm::vec2(2.0f, 1.0f)),
        Block(glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 2.0f))  // Stone block
    };

    enum BLOCKS {
        AIR = 0,
        DIRT_BLOCK = 1,
        GRASS_BLOCK = 2,
        STONE_BLOCK = 3
    };
}
