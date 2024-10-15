// chunk.h
#ifndef CHUNK_H
#define CHUNK_H

#include <vector>
#include <glm/glm.hpp>
#include "vao.h"
#include "vbo.h"
#include "ebo.h"
#include "shader.h"
#include "texture.h"
#include "camera.h"

class Chunk {
public:
    Chunk(int width, int height, int depth);
    void render(Shader& shader, Texture& texture, Camera& camera);

private:
    int width, height, depth;
    std::vector<std::vector<std::vector<bool>>> chunkData;
    std::vector<glm::vec3> cubePositions;
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;
    VAO vao;
    VBO vbo;
    EBO ebo;

    void generateChunk();
    void setupBuffers();
    bool isFaceVisible(int x, int y, int z, int face);
    bool isCubeAt(int x, int y, int z);
};

#endif
