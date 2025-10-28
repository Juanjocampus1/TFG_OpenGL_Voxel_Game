#include "Camera.h"

Camera::Camera(float width, float height)
    : position(8.0f, 5.0f, 20.0f), front(0.0f, 0.0f, -1.0f), up(0.0f, 1.0f, 0.0f),
      right(1.0f, 0.0f, 0.0f), worldUp(0.0f, 1.0f, 0.0f), yaw(-90.0f), pitch(-10.0f),
      movementSpeed(2.5f), mouseSensitivity(0.1f), width(width), height(height) {}

Camera::~Camera() {}

void Camera::Update(float dt) {
    // Update vectors
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    this->front = glm::normalize(front);
    right = glm::normalize(glm::cross(this->front, worldUp));
    up = glm::normalize(glm::cross(right, this->front));
}

glm::mat4 Camera::GetViewMatrix() const {
    return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::GetProjectionMatrix() const {
    return glm::perspective(glm::radians(45.0f), width / height, 0.1f, 100.0f);
}

void Camera::ProcessKeyboard(KeyCode key, float dt) {
    float velocity = movementSpeed * dt * 6.5f;
    if (key == KeyCode::W) position += front * velocity;
    if (key == KeyCode::S) position -= front * velocity;
    if (key == KeyCode::A) position -= right * velocity;
    if (key == KeyCode::D) position += right * velocity;
    if (key == KeyCode::Space) position += worldUp * velocity;
    if (key == KeyCode::LeftShift) position -= worldUp * velocity;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset) {
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw += xoffset;
    pitch -= yoffset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;
}