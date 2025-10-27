#include "Window.h"
#include "Event.h"
#include <iostream>

Window::Window(const char* title, int width, int height) : width(width), height(height), mouseCaptured(false) {
    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW" << std::endl;
        exit(-1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }

    glfwMakeContextCurrent(window);
    // Disable VSync
    glfwSwapInterval(0);

    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    lastX = width / 2.0;
    lastY = height / 2.0;
}

Window::~Window() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Window::SetMouseCapture(bool capture) {
    mouseCaptured = capture;
    if (capture) {
        glfwSetCursorPos(window, lastX, lastY);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

void Window::Update() {
    glfwSwapBuffers(window);
    glfwPollEvents();
}

void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (win && win->resizeCallback) {
        win->resizeCallback(width, height);
    }
}

void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (win && win->eventCallback) {
        if (action == GLFW_PRESS) {
            KeyPressedEvent e(static_cast<KeyCode>(key), 0);
            win->eventCallback(e);
        } else if (action == GLFW_RELEASE) {
            KeyReleasedEvent e(static_cast<KeyCode>(key));
            win->eventCallback(e);
        }
    }
}

void Window::cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) {
    Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (win && win->eventCallback) {
        double deltaX = xpos - win->lastX;
        double deltaY = ypos - win->lastY;
        MouseMovedEvent e(static_cast<float>(deltaX), static_cast<float>(deltaY));
        win->eventCallback(e);
        win->lastX = xpos;
        win->lastY = ypos;
    }
}

void Window::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (win && win->eventCallback) {
        if (action == GLFW_PRESS) {
            MouseButtonPressedEvent e(button);
            win->eventCallback(e);
        }
    }
}