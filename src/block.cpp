#include "Header_files/Block.h"

Block::Block(glm::vec2 min, glm::vec2 max)
    : topMin(min), topMax(max), bottomMin(min), bottomMax(max), sideMin(min), sideMax(max) {}

Block::Block(glm::vec2 topMin, glm::vec2 topMax,
    glm::vec2 bottomMin, glm::vec2 bottomMax,
    glm::vec2 sideMin, glm::vec2 sideMax)
    : topMin(topMin), topMax(topMax), bottomMin(bottomMin), bottomMax(bottomMax), sideMin(sideMin), sideMax(sideMax) {}
