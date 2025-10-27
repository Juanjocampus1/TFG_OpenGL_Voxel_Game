#include "Application.h"

Application::Application() : m_Window(nullptr), m_Renderer(nullptr), shader(nullptr), texture(nullptr), camera(nullptr), planet(nullptr), lastTime(0.0), frameCount(0), fps(0.0), timestep(0.0f) {
    m_Window = new Window("Red Triangle", 800, 600);
    m_Window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));
    m_Window->SetResizeCallback([this](int w, int h) {
        camera->SetSize(w, h);
    });
    initGLAD();
    camera = new Camera(m_Window->GetWidth(), m_Window->GetHeight());
    imGuiLayer = new ImGuiLayer(m_Window->GetNativeWindow());
    m_Renderer = new Renderer();
    createShaders();
    createTexture();
    planet = new Planet(3); // Back to render distance 3 with optimizations
}

Application::~Application() {
    cleanup();
}

void Application::run() {
    lastTime = glfwGetTime();
    double fpsTimer = 0.0;
    while (!glfwWindowShouldClose(m_Window->GetNativeWindow())) {
        double currentTime = glfwGetTime();
        timestep = (float)(currentTime - lastTime);
        lastTime = currentTime;
        fpsTimer += timestep;
        frameCount++;
        if (fpsTimer >= 1.0) {
            fps = frameCount / fpsTimer;
            frameCount = 0;
            fpsTimer = 0.0;
        }
        render();
        m_Window->Update();
    }
}

void Application::OnEvent(Event& e) {
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<KeyPressedEvent>([this](KeyPressedEvent& event) {
        KeyCode key = event.GetKeyCode();
        if (key == KeyCode::Escape) {
            m_Window->SetMouseCapture(false);
            return false;
        }
        pressedKeys.insert(key);
        return false;
    });
    dispatcher.Dispatch<KeyReleasedEvent>([this](KeyReleasedEvent& event) {
        pressedKeys.erase(event.GetKeyCode());
        return false;
    });
    dispatcher.Dispatch<MouseButtonPressedEvent>([this](MouseButtonPressedEvent& event) {
        if (event.GetButton() == 0) { // left button
            m_Window->SetMouseCapture(true);
        }
        return false;
    });
    dispatcher.Dispatch<MouseMovedEvent>([this](MouseMovedEvent& event) {
        if (m_Window->IsMouseCaptured()) {
            camera->ProcessMouseMovement(event.GetX(), event.GetY());
        }
        return false;
    });
}

void Application::initGLAD() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(-1);
    }
    glViewport(0, 0, m_Window->GetWidth(), m_Window->GetHeight());
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}

void Application::createShaders() {
    shader = new Shader("shaders/MineShader.glsl");
}

void Application::createTexture() {
    texture = new Texture("Texture_atlas.png", "diffuse", 0);
}

void Application::render() {
    camera->Update(timestep);

    // Process continuous key presses
    for (KeyCode key : pressedKeys) {
        camera->ProcessKeyboard(key, timestep);
    }

    m_Renderer->BeginScene();
    texture->texUnit(*shader, "tex0", 0);
    texture->bind();

    planet->setCameraPos(camera->GetPosition());
    planet->updateChunks();
    planet->draw(*shader, *camera);

    texture->unbind();

    imGuiLayer->begin();
    imGuiLayer->renderPanel(fps, planet->loaded, planet->unloaded);
    imGuiLayer->end();

    m_Renderer->EndScene();
}

void Application::cleanup() {
    if (texture) texture->deleteTex();
    delete shader;
    delete texture;
    delete planet;
    delete imGuiLayer;
    delete m_Renderer;
    delete m_Window;
    delete camera;
}