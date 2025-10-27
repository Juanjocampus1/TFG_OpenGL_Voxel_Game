#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include "../Core/KeyCodes.h"

class Camera {
public:
    Camera(float width, float height);
    ~Camera();

    void Update(float dt);
    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix() const;

    void ProcessKeyboard(KeyCode key, float dt);
    void ProcessMouseMovement(float xoffset, float yoffset);
    void SetSize(float w, float h) { width = w; height = h; }

private:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;

    float yaw;
    float pitch;

    float movementSpeed;
    float mouseSensitivity;

    float width, height;
};

#endif