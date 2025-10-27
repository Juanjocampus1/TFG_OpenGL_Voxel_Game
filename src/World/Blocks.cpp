#include "Blocks.h"

std::string Blocks::getName(BlockType type) {
    switch (type) {
    case BlockType::AIR: return "Air";
    case BlockType::GRASS: return "Grass";
    case BlockType::DIRT: return "Dirt";
    case BlockType::STONE: return "Stone";
    default: return "Unknown";
    }
}

bool Blocks::isSolid(BlockType type) {
    return type != BlockType::AIR;
}