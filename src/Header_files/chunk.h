#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include "Camera.h"
#include "Shader.h"
#include "texture.h"

struct ChunkVertex {

    glm::vec3 position;
    glm::vec2 texCoords;

    ChunkVertex(glm::vec3 pos, glm::vec2 tex)
        : position(pos), texCoords(tex) {}
};

class Chunk {
public:
    Chunk(unsigned int chunkSize, glm::vec3 chunkPos);
    ~Chunk();

    void GenerateChunk();
    void Render(Shader& shader, Camera& camera);
    int GetCubeCount() const;
    static int GetChunkCount();

public:
    std::vector<unsigned int> chunkData;
    glm::vec3 chunkPos;
    bool ready;
    bool generated;

private:
    unsigned int chunkSize;
    unsigned int numTriangles;
    unsigned int vao, vbo, ebo;
    static int chunkCount;
};
