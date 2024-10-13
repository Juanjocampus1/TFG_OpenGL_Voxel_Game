#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

class Shader {
public:
    unsigned int ID;

    Shader(const char* vertexPath, const char* fragmentPath); // Solo la declaración

    void use();
    void setMat4(const std::string& name, const glm::mat4& mat) const;
};

#endif
