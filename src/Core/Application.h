#ifndef APPLICATION_H
#define APPLICATION_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <set>
#include "Event.h"
#include "../Renderer/Shader.h"
#include "../Renderer/VAO.h"
#include "../Renderer/VBO.h"
#include "../Renderer/EBO.h"
#include "../Renderer/Texture.h"
#include "../Renderer/Camera.h"
#include "../Renderer/Renderer.h"
#include "../Renderer/ImGuiLayer.h"
#include "../Game/Cube.h"
#include "../World/Planet.h"
#include "Window.h"
#include "Timestep.h"

class Application {
public:
    Application();
    ~Application();
    void run();

    void OnEvent(Event& e);

private:
    Window* m_Window;
    Renderer* m_Renderer;
    Shader* shader;
    Texture* texture;
    Camera* camera;
    Planet* planet;
    ImGuiLayer* imGuiLayer;

    double lastTime;
    int frameCount;
    double fps;
    Timestep timestep;

    std::set<KeyCode> pressedKeys;
    bool mouseCaptured;

    void initGLAD();
    void createShaders();
    void createBuffers();
    void createTexture();
    void createCube();
    void render();
    void cleanup();
};

#endif