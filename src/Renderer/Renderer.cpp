#include "Renderer.h"
#include "../Core/Time.h"
#include <glad/glad.h>

Renderer::Renderer() {}

Renderer::~Renderer() {}

void Renderer::BeginScene() {
    // Get time of day from global Time system
    float timeOfDay = Time::Get().getTime();

    // Compute sky color based on time of day
    // timeOfDay in [0,1]: 0=midnight, 0.25=sunrise, 0.5=noon, 0.75=sunset, 1=midnight
    float t = timeOfDay;
    glm::vec3 skyColor;
    
    if (t < 0.25f) {
        // Night -> sunrise
        float u = t / 0.25f;
        skyColor = glm::mix(glm::vec3(0.02f, 0.03f, 0.08f), glm::vec3(0.6f, 0.4f, 0.3f), u);
    } else if (t < 0.5f) {
        // Sunrise -> day
        float u = (t - 0.25f) / 0.25f;
        skyColor = glm::mix(glm::vec3(0.6f, 0.4f, 0.3f), glm::vec3(0.53f, 0.81f, 0.92f), u);
    } else if (t < 0.75f) {
        // Day -> sunset
        float u = (t - 0.5f) / 0.25f;
        skyColor = glm::mix(glm::vec3(0.53f, 0.81f, 0.92f), glm::vec3(0.6f, 0.4f, 0.3f), u);
    } else {
        // Sunset -> night
        float u = (t - 0.75f) / 0.25f;
        skyColor = glm::mix(glm::vec3(0.6f, 0.4f, 0.3f), glm::vec3(0.02f, 0.03f, 0.08f), u);
    }

    glClearColor(skyColor.r, skyColor.g, skyColor.b, 1.0f);
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