#ifndef CHUNK_H
#define CHUNK_H

#include "Block.h"
#include "../Renderer/VAO.h"
#include "../Renderer/VBO.h"
#include "../Renderer/EBO.h"
#include <vector>
#include <glm/glm.hpp>

class Chunk {
public:
    static const int SIZE = 16;

    Block blocks[SIZE][SIZE][SIZE];

    Chunk();
    ~Chunk();
    void generate();
    void buildMesh();
    void draw(class Shader& shader, class Camera& camera);

private:
    VAO* vao;
    VBO* vbo;
    EBO* ebo;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
};

#endif