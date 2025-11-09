#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>

class Shader {
public:
    unsigned int ID;

    Shader(const char* shaderPath);
    void use() const;
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec2(const std::string& name, const glm::vec2& v) const;
    void setVec3(const std::string& name, const glm::vec3& v) const;
    void setVec4(const std::string& name, const glm::vec4& v) const;
    void setMat3(const std::string& name, const glm::mat3& mat) const;
    void setMat4(const std::string& name, const glm::mat4& mat) const;

private:
    std::pair<std::string, std::string> splitShaders(const std::string& source);
};

#endif