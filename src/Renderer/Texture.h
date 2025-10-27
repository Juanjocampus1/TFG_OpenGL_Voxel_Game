#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <string>
#include "Shader.h"

class Texture {
public:
    GLuint ID;
    std::string type;
    GLuint unit;

    Texture(const char* image, const char* texType, GLuint slot);
    void texUnit(Shader& shader, const char* uniform, GLuint unit);
    void bind();
    void unbind();
    void deleteTex();

private:
    int widthImg, heightImg, numColCh;
};

#endif