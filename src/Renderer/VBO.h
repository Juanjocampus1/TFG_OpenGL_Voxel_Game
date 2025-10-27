#ifndef VBO_H
#define VBO_H

#include <glad/glad.h>

class VBO {
public:
    GLuint ID;

    VBO(GLfloat* vertices, GLsizeiptr size);
    void bind();
    void unbind();
    void deleteBuffer();
};

#endif