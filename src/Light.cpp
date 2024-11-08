#include "Header_files/Light.h"

Light::Light(glm::vec3 position, glm::vec4 color, std::vector<Vertex>& vertices, std::vector<GLuint>& indices, std::vector<Texture>& textures)
    : position(position), color(color) {
    model = glm::mat4(1.0f);
    updateModel();
    mesh = new Mesh(vertices, indices, textures);
}

Light::~Light() {
    delete mesh;
}

void Light::setPosition(glm::vec3 newPosition) {
    position = newPosition;
    updateModel();
}

void Light::setColor(glm::vec4 newColor) {
    color = newColor;
}

void Light::updateModel() {
    model = glm::mat4(1.0f);
    model = glm::translate(model, position);
}

void Light::render(Shader& shader, Camera& camera) {
    shader.use();
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform4f(glGetUniformLocation(shader.ID, "lightColor"), color.r, color.g, color.b, color.a);
    mesh->Draw(shader, camera);
}
