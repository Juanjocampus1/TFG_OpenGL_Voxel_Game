#ifndef EBO_H
#define EBO_H

#include <glad/glad.h>

class EBO {
public:
    GLuint ID;

    EBO(GLuint* indices, GLsizeiptr size);
    void bind();
    void unbind();
    void deleteEBO();
};

#endif