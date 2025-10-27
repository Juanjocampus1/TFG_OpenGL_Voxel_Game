#ifndef IMGUI_LAYER_H
#define IMGUI_LAYER_H

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

class ImGuiLayer {
public:
    ImGuiLayer(GLFWwindow* window);
    ~ImGuiLayer();
    void begin();
    void end();
    void renderPanel(double fps, int loaded, int unloaded);
};

#endif