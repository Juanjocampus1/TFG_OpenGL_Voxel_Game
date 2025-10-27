#include "Renderer.h"
#include <glad/glad.h>

Renderer::Renderer() {}

Renderer::~Renderer() {}

void Renderer::BeginScene() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}

void Renderer::EndScene() {}

void Renderer::Submit(const VAO& vao, const Shader& shader, unsigned int count, bool indexed) {
    shader.use();
    vao.bind();
    if (indexed) {
        glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, 0);
    } else {
        glDrawArrays(GL_TRIANGLES, 0, count);
    }
}