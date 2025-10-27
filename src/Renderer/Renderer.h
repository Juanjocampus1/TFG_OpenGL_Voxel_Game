#ifndef RENDERER_H
#define RENDERER_H

#include "Shader.h"
#include "VAO.h"
#include "VBO.h"

class Renderer {
public:
    Renderer();
    ~Renderer();

    void BeginScene();
    void EndScene();
    void Submit(const VAO& vao, const Shader& shader, unsigned int count, bool indexed = false);

private:
    // For now, simple
};

#endif