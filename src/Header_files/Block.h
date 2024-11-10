#pragma once

#include <glm/glm.hpp>

struct Block {
public:
    glm::vec2 topMin;
    glm::vec2 topMax;
    glm::vec2 bottomMin;
    glm::vec2 bottomMax;
    glm::vec2 sideMin;
    glm::vec2 sideMax;

    Block(glm::vec2 min, glm::vec2 max);
    Block(glm::vec2 topMin, glm::vec2 topMax,
        glm::vec2 bottomMin, glm::vec2 bottomMax,
        glm::vec2 sideMin, glm::vec2 sideMax);
};
