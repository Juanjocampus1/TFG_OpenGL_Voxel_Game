#ifndef WINDOW_H
#define WINDOW_H

#include <GLFW/glfw3.h>
#include <functional>

class Window {
public:
    Window(const char* title, int width, int height);
    ~Window();

    GLFWwindow* GetNativeWindow() const { return window; }
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    void SetMouseCapture(bool capture);
    bool IsMouseCaptured() const { return mouseCaptured; }
    void SetEventCallback(const std::function<void(class Event&)>& callback) { eventCallback = callback; }
    void SetResizeCallback(std::function<void(int, int)> callback) { resizeCallback = callback; }
    void Update();

private:
    GLFWwindow* window;
    int width, height;
    bool mouseCaptured;
    std::function<void(class Event&)> eventCallback;
    std::function<void(int, int)> resizeCallback;
    double lastX, lastY;

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
};

#endif