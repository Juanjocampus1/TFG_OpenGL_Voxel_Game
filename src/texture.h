#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <stb/stb_image.h>
#include "shader.h"

class Texture {
    public:
        GLuint ID; // Declaración de ID
        GLenum type;
        GLuint unit;
        
        Texture(const char* image, GLenum texType, GLuint unit, GLenum format, GLenum pixelType);

        void texUnit(Shader& shader, const char* uniform, GLuint unit);
        void Bind();
        void Unbind();
        void Delete();
};

#endif
