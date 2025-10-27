#include "Chunk.h"
#include "../Renderer/Shader.h"
#include "../Renderer/Camera.h"
#include <glad/glad.h>

Chunk::Chunk() : vao(nullptr), vbo(nullptr), ebo(nullptr) {
    generate();
}

Chunk::~Chunk() {
    delete vao;
    delete vbo;
    delete ebo;
}

void Chunk::generate() {
    for (int x = 0; x < SIZE; ++x) {
        for (int z = 0; z < SIZE; ++z) {
            for (int y = 0; y < SIZE; ++y) {
                if (y < 4) {
                    blocks[x][y][z] = Block(BlockType::STONE);
                } else if (y < 8) {
                    blocks[x][y][z] = Block(BlockType::DIRT);
                } else if (y == 8) {
                    blocks[x][y][z] = Block(BlockType::GRASS);
                } else {
                    blocks[x][y][z] = Block(BlockType::AIR);
                }
            }
        }
    }
    buildMesh();
}

void Chunk::buildMesh() {
    vertices.clear();
    indices.clear();
    unsigned int vertexCount = 0;

    for (int x = 0; x < SIZE; ++x) {
        for (int y = 0; y < SIZE; ++y) {
            for (int z = 0; z < SIZE; ++z) {
                if (blocks[x][y][z].type == BlockType::AIR) continue;

                BlockType type = blocks[x][y][z].type;
                float offsetU = 0.0f, offsetV = 0.0f;
                if (type == BlockType::GRASS) {
                    offsetU = 0.5f; offsetV = 0.5f;
                } else if (type == BlockType::DIRT) {
                    offsetU = 0.0f; offsetV = 0.5f;
                } else if (type == BlockType::STONE) {
                    offsetU = 0.0f; offsetV = 0.0f;
                }

                // Front face (z+)
                if (z + 1 >= SIZE || blocks[x][y][z + 1].type == BlockType::AIR) {
                    vertices.insert(vertices.end(), {
                        x - 0.5f, y - 0.5f, z + 0.5f, 0.5f + offsetU, 0.0f + offsetV,
                        x + 0.5f, y - 0.5f, z + 0.5f, 1.0f + offsetU, 0.0f + offsetV,
                        x + 0.5f, y + 0.5f, z + 0.5f, 1.0f + offsetU, 0.5f + offsetV,
                        x - 0.5f, y + 0.5f, z + 0.5f, 0.5f + offsetU, 0.5f + offsetV
                    });
                    indices.insert(indices.end(), {
                        vertexCount, vertexCount + 1, vertexCount + 2,
                        vertexCount + 2, vertexCount + 3, vertexCount
                    });
                    vertexCount += 4;
                }

                // Back face (z-)
                if (z - 1 < 0 || blocks[x][y][z - 1].type == BlockType::AIR) {
                    vertices.insert(vertices.end(), {
                        x - 0.5f, y - 0.5f, z - 0.5f, 0.5f + offsetU, 0.0f + offsetV,
                        x - 0.5f, y + 0.5f, z - 0.5f, 0.5f + offsetU, 0.5f + offsetV,
                        x + 0.5f, y + 0.5f, z - 0.5f, 1.0f + offsetU, 0.5f + offsetV,
                        x + 0.5f, y - 0.5f, z - 0.5f, 1.0f + offsetU, 0.0f + offsetV
                    });
                    indices.insert(indices.end(), {
                        vertexCount, vertexCount + 1, vertexCount + 2,
                        vertexCount + 2, vertexCount + 3, vertexCount
                    });
                    vertexCount += 4;
                }

                // Left face (x-)
                if (x - 1 < 0 || blocks[x - 1][y][z].type == BlockType::AIR) {
                    vertices.insert(vertices.end(), {
                        x - 0.5f, y + 0.5f, z + 0.5f, 0.5f + offsetU, 0.5f + offsetV,
                        x - 0.5f, y + 0.5f, z - 0.5f, 1.0f + offsetU, 0.5f + offsetV,
                        x - 0.5f, y - 0.5f, z - 0.5f, 1.0f + offsetU, 0.0f + offsetV,
                        x - 0.5f, y - 0.5f, z + 0.5f, 0.5f + offsetU, 0.0f + offsetV
                    });
                    indices.insert(indices.end(), {
                        vertexCount, vertexCount + 1, vertexCount + 2,
                        vertexCount + 2, vertexCount + 3, vertexCount
                    });
                    vertexCount += 4;
                }

                // Right face (x+)
                if (x + 1 >= SIZE || blocks[x + 1][y][z].type == BlockType::AIR) {
                    vertices.insert(vertices.end(), {
                        x + 0.5f, y + 0.5f, z + 0.5f, 0.5f + offsetU, 0.5f + offsetV,
                        x + 0.5f, y - 0.5f, z + 0.5f, 0.5f + offsetU, 0.0f + offsetV,
                        x + 0.5f, y - 0.5f, z - 0.5f, 1.0f + offsetU, 0.0f + offsetV,
                        x + 0.5f, y + 0.5f, z - 0.5f, 1.0f + offsetU, 0.5f + offsetV
                    });
                    indices.insert(indices.end(), {
                        vertexCount, vertexCount + 1, vertexCount + 2,
                        vertexCount + 2, vertexCount + 3, vertexCount
                    });
                    vertexCount += 4;
                }

                // Top face (y+)
                if (y + 1 >= SIZE || blocks[x][y + 1][z].type == BlockType::AIR) {
                    vertices.insert(vertices.end(), {
                        x - 0.5f, y + 0.5f, z - 0.5f, 0.5f + offsetU, 0.5f + offsetV,
                        x - 0.5f, y + 0.5f, z + 0.5f, 0.5f + offsetU, 1.0f + offsetV,
                        x + 0.5f, y + 0.5f, z + 0.5f, 1.0f + offsetU, 1.0f + offsetV,
                        x + 0.5f, y + 0.5f, z - 0.5f, 1.0f + offsetU, 0.5f + offsetV
                    });
                    indices.insert(indices.end(), {
                        vertexCount, vertexCount + 1, vertexCount + 2,
                        vertexCount + 2, vertexCount + 3, vertexCount
                    });
                    vertexCount += 4;
                }

                // Bottom face (y-)
                if (y - 1 < 0 || blocks[x][y - 1][z].type == BlockType::AIR) {
                    vertices.insert(vertices.end(), {
                        x - 0.5f, y - 0.5f, z - 0.5f, 0.0f + offsetU, 0.0f + offsetV,
                        x + 0.5f, y - 0.5f, z - 0.5f, 0.5f + offsetU, 0.0f + offsetV,
                        x + 0.5f, y - 0.5f, z + 0.5f, 0.5f + offsetU, 0.5f + offsetV,
                        x - 0.5f, y - 0.5f, z + 0.5f, 0.0f + offsetU, 0.5f + offsetV
                    });
                    indices.insert(indices.end(), {
                        vertexCount, vertexCount + 1, vertexCount + 2,
                        vertexCount + 2, vertexCount + 3, vertexCount
                    });
                    vertexCount += 4;
                }
            }
        }
    }

    if (!vertices.empty()) {
        vao = new VAO();
        vbo = new VBO(vertices.data(), vertices.size() * sizeof(float));
        ebo = new EBO(indices.data(), indices.size() * sizeof(unsigned int));

        vao->bind();
        vao->linkVBO(*vbo, 0, 3, GL_FLOAT, 5 * sizeof(float), (void*)0);
        vao->linkVBO(*vbo, 1, 2, GL_FLOAT, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        ebo->bind();
        vao->unbind();
    }
}

void Chunk::draw(Shader& shader, Camera& camera) {
    if (!vao) return;
    shader.use();
    shader.setMat4("model", glm::mat4(1.0f));
    shader.setMat4("view", camera.GetViewMatrix());
    shader.setMat4("projection", camera.GetProjectionMatrix());
    vao->bind();
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}