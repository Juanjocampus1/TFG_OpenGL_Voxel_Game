#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "Mesh.h"

class Light {
public:
    glm::vec4 color;
    glm::vec3 position;
    glm::mat4 model;
    Mesh* mesh;

    Light(glm::vec3 position, glm::vec4 color, std::vector<Vertex>& vertices, std::vector<GLuint>& indices, std::vector<Texture>& textures);
    ~Light();
    void setPosition(glm::vec3 newPosition);
    void setColor(glm::vec4 newColor);
    void updateModel();
    void render(Shader& shader, Camera& camera);
};

#endif // LIGHT_H
