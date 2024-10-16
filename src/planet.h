#ifndef PLANETA_H
#define PLANETA_H

#include <vector>
#include <glm/glm.hpp>
#include "chunk.h"
#include "shader.h"
#include "texture.h"
#include "camera.h"

class Planeta {
public:
    Planeta(int renderDistance);
    void generateWorld(int chunkWidth, int chunkHeight, int chunkDepth);
    void renderWorld(Shader& shader, Texture& texture, Camera& camera, glm::vec3 playerPosition);
    void updateChunks(glm::vec3 playerPosition);
    bool isChunkInRenderDistance(glm::vec3 chunkPosition, glm::vec3 playerPosition);

private:
    int renderDistance;
    std::vector<Chunk> chunks;
    std::vector<glm::vec3> chunkPositions;
};

#endif
