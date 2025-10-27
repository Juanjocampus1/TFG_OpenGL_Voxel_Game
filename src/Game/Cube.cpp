#include "Cube.h"
#include <glad/glad.h>
#include "../Renderer/Shader.h"
#include "../Renderer/Camera.h"

Cube::Cube(float x, float y, float z, BlockType type) : vao(nullptr), vbo(nullptr), ebo(nullptr), posX(x), posY(y), posZ(z), blockType(type) {
    float offsetU = 0.0f;
    float offsetV = 0.0f;
    if (blockType == BlockType::GRASS) {
        offsetU = 0.5f;
        offsetV = 0.5f;
    } else if (blockType == BlockType::DIRT) {
        offsetU = 0.0f;
        offsetV = 0.5f;
    } else if (blockType == BlockType::STONE) {
        offsetU = 0.0f;
        offsetV = 0.0f;
    }

    // Cube vertices with UV for atlas, adjusted by offset
    float vertices[] = {
        // Front face
        -0.5f, -0.5f,  0.5f,  0.5f + offsetU, 0.0f + offsetV,
         0.5f, -0.5f,  0.5f,  1.0f + offsetU, 0.0f + offsetV,
         0.5f,  0.5f,  0.5f,  1.0f + offsetU, 0.5f + offsetV,
        -0.5f,  0.5f,  0.5f,  0.5f + offsetU, 0.5f + offsetV,
        // Back face
        -0.5f, -0.5f, -0.5f,  0.5f + offsetU, 0.0f + offsetV,
        -0.5f,  0.5f, -0.5f,  0.5f + offsetU, 0.5f + offsetV,
         0.5f,  0.5f, -0.5f,  1.0f + offsetU, 0.5f + offsetV,
         0.5f, -0.5f, -0.5f,  1.0f + offsetU, 0.0f + offsetV,
        // Left face
        -0.5f,  0.5f,  0.5f,  0.5f + offsetU, 0.5f + offsetV,
        -0.5f,  0.5f, -0.5f,  1.0f + offsetU, 0.5f + offsetV,
        -0.5f, -0.5f, -0.5f,  1.0f + offsetU, 0.0f + offsetV,
        -0.5f, -0.5f,  0.5f,  0.5f + offsetU, 0.0f + offsetV,
        // Right face
         0.5f,  0.5f,  0.5f,  0.5f + offsetU, 0.5f + offsetV,
         0.5f, -0.5f,  0.5f,  0.5f + offsetU, 0.0f + offsetV,
         0.5f, -0.5f, -0.5f,  1.0f + offsetU, 0.0f + offsetV,
         0.5f,  0.5f, -0.5f,  1.0f + offsetU, 0.5f + offsetV,
        // Top face
        -0.5f,  0.5f, -0.5f,  0.5f + offsetU, 0.5f + offsetV,
        -0.5f,  0.5f,  0.5f,  0.5f + offsetU, 1.0f + offsetV,
         0.5f,  0.5f,  0.5f,  1.0f + offsetU, 1.0f + offsetV,
         0.5f,  0.5f, -0.5f,  1.0f + offsetU, 0.5f + offsetV,
        // Bottom face
        -0.5f, -0.5f, -0.5f,  0.0f + offsetU, 0.0f + offsetV,
         0.5f, -0.5f, -0.5f,  0.5f + offsetU, 0.0f + offsetV,
         0.5f, -0.5f,  0.5f,  0.5f + offsetU, 0.5f + offsetV,
        -0.5f, -0.5f,  0.5f,  0.0f + offsetU, 0.5f + offsetV
    };

    unsigned int indices[] = {
        // Front
        0, 1, 2, 2, 3, 0,
        // Back
        4, 5, 6, 6, 7, 4,
        // Left
        8, 9, 10, 10, 11, 8,
        // Right
        12, 13, 14, 14, 15, 12,
        // Top
        16, 17, 18, 18, 19, 16,
        // Bottom
        20, 21, 22, 22, 23, 20
    };

    vao = new VAO();
    vbo = new VBO(vertices, sizeof(vertices));
    ebo = new EBO(indices, sizeof(indices));

    vao->bind();
    vao->linkVBO(*vbo, 0, 3, GL_FLOAT, 5 * sizeof(float), (void*)0);
    vao->linkVBO(*vbo, 1, 2, GL_FLOAT, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    ebo->bind();
    vao->unbind();
}

Cube::~Cube() {
    delete vao;
    delete vbo;
    delete ebo;
}

void Cube::Draw(Shader& shader, Camera& camera) {
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(posX, posY, posZ));
    shader.setMat4("model", model);
    shader.setMat4("view", camera.GetViewMatrix());
    shader.setMat4("projection", camera.GetProjectionMatrix());
    vao->bind();
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}