#include "ImGuiLayer.h"

ImGuiLayer::ImGuiLayer(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

ImGuiLayer::~ImGuiLayer() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiLayer::begin() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiLayer::end() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiLayer::renderPanel(double fps, int loaded, int unloaded) {
    ImGui::Begin("Debug Panel");
    ImGui::Text("FPS: %.1f", fps);
    ImGui::Text("Chunks Loaded: %d", loaded);
    ImGui::Text("Chunks Unloaded: %d", unloaded);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / fps, fps);
    ImGui::End();
}