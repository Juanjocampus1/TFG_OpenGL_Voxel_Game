#ifndef BLOCK_H
#define BLOCK_H

enum class BlockType {
    AIR,
    GRASS,
    DIRT,
    STONE
};

class Block {
public:
    BlockType type;

    Block(BlockType t = BlockType::AIR) : type(t) {}
};

#endif