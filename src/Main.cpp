#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Header_files/Mesh.h"
#include "Header_files/Chunk.h"

using namespace std;

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

// Variables para la luz
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
float lightIntensity = 1.0f;

// Variable para escalar el cubo
float cubeScale = 1.0f;

Vertex vertices[] = {
    // posiciones                            // colores              // coordenadas de textura (U, V)             //normals
    // Cara trasera
    {glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(16.0f / 32.0f, 0.0f / 32.0f)},
    {glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(32.0f / 32.0f, 0.0f / 32.0f)},
    {glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(32.0f / 32.0f, 16.0f / 32.0f)},
    {glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(1.0f, 1.0f, 0.0f),  glm::vec2(16.0f / 32.0f, 16.0f / 32.0f)},

    // Cara delantera
    {glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec2(16.0f / 32.0f, 0.0f / 32.0f)},
    {glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 1.0f), glm::vec2(32.0f / 32.0f, 0.0f / 32.0f)},
    {glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(32.0f / 32.0f, 16.0f / 32.0f)},
    {glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(16.0f / 32.0f, 16.0f / 32.0f)},

    // Cara izquierda
    {glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(16.0f / 32.0f, 0.0f / 32.0f)},
    {glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec2(32.0f / 32.0f, 0.0f / 32.0f)},
    {glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(32.0f / 32.0f, 16.0f / 32.0f)},
    {glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(16.0f / 32.0f, 16.0f / 32.0f)},

    // Cara derecha
    {glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(16.0f / 32.0f, 0.0f / 32.0f)},
    {glm::vec3(0.5f, -0.5f, 0.5f),glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 1.0f), glm::vec2(32.0f / 32.0f, 0.0f / 32.0f)},
    {glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(32.0f / 32.0f, 16.0f / 32.0f)},
    {glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(16.0f / 32.0f, 16.0f / 32.0f)},

    // Cara inferior
    {glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f / 32.0f, 1.0f - 32.0f / 32.0f)},
    {glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(16.0f / 32.0f, 1.0f - 32.0f / 32.0f)},
    {glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(16.0f / 32.0f, 1.0f - 32.0f / 32.0f)},
    {glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(0.0f / 32.0f, 1.0f - 32.0f / 32.0f)},

    // Cara superior
    {glm::vec3(-0.5f, 0.5f, -0.5f),  glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(16.0f / 32.0f, 1.0f - 0.0f / 32.0f)},
    {glm::vec3(0.5f, 0.5f, -0.5f),  glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(32.0f / 32.0f, 1.0f - 0.0f / 32.0f)},
    {glm::vec3(0.5f, 0.5f, 0.5f),  glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(32.0f / 32.0f, 1.0f - 16.0f / 32.0f)},
    {glm::vec3(-0.5f, 0.5f, 0.5f),  glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(16.0f / 32.0f, 1.0f - 16.0f / 32.0f)}
};

GLuint indices[] = {
    0, 1, 2, 0, 2, 3,   // Cara trasera
    4, 7, 6, 4, 6, 5,   // Cara delantera (ajustada)
    8, 11, 10, 8, 10, 9,   // Cara izquierda (ajustada)
    12, 13, 14, 12, 14, 15,   // Cara derecha
    16, 19, 18, 16, 18, 17,   // Cara inferior (ajustada)
    20, 21, 22, 20, 22, 23    // Cara superior
};

Vertex lightVertices[] = {
    // Posiciones (x, y, z)
    Vertex{glm::vec3(-0.1f, -0.1f,  0.1f)},
    Vertex{glm::vec3(-0.1f, -0.1f, -0.1f)},
    Vertex{glm::vec3(0.1f, -0.1f, -0.1f)},
    Vertex{glm::vec3(0.1f, -0.1f,  0.1f)},
    Vertex{glm::vec3(-0.1f,  0.1f,  0.1f)},
    Vertex{glm::vec3(-0.1f,  0.1f, -0.1f)},
    Vertex{glm::vec3(0.1f,  0.1f, -0.1f)},
    Vertex{glm::vec3(0.1f,  0.1f,  0.1f)}
};

GLuint lightIndices[] = {
    0, 1, 2,
    0, 2, 3,
    0, 4, 7,
    0, 7, 3,
    3, 7, 6,
    3, 6, 2,
    2, 6, 5,
    2, 5, 1,
    1, 5, 4,
    1, 4, 0,
    4, 5, 6,
    4, 6, 7
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main() {
    // Inicializaci�n de GLFW
    if (!glfwInit()) {
        cout << "Fall� la inicializaci�n de GLFW" << endl;
        return -1;
    }

    // Configuraci�n de la versi�n de OpenGL y perfil
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Creaci�n de la ventana
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Voxel Game Engine", NULL, NULL);
    if (window == NULL) {
        cout << "Fall� la creaci�n de la ventana GLFW" << endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cout << "Fall� la inicializaci�n de GLAD" << endl;
        return -1;
    }

    // Ajuste del tama�o del viewport
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    Texture textures[]
    {
        Texture("Texture_atlas.png", "diffuse", 0, GL_RGBA, GL_UNSIGNED_BYTE),
        Texture("Texture_atlas_specular.png", "specular", 1, GL_RED, GL_UNSIGNED_BYTE)
    };

    // draw a cube
    Shader shaderProgram("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");
    std::vector <Vertex> verts(vertices, vertices + sizeof(vertices) / sizeof(Vertex));
    std::vector <GLuint> ind(indices, indices + sizeof(indices) / sizeof(GLuint));
    std::vector <Texture> tex(textures, textures + sizeof(textures) / sizeof(Texture));
    Mesh Cube(verts, ind, tex);
    Chunk chunk(16, 1, 16, verts, ind, tex);

    //draw a light cube
    Shader lightShader("shaders/light_vertex_shader.glsl", "shaders/light_fragment_shader.glsl");
    std::vector <Vertex> lightVerts(lightVertices, lightVertices + sizeof(lightVertices) / sizeof(Vertex));
    std::vector <GLuint> lightInd(lightIndices, lightIndices + sizeof(lightIndices) / sizeof(GLuint));
    Mesh light(lightVerts, lightInd, tex);

    glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec3 lightPos = glm::vec3(0.0f, 2.0f, 0.0f);
    //glm::vec3 lightPos = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::mat4 lightModel = glm::mat4(1.0f);
    lightModel = glm::translate(lightModel, lightPos);

    glm::vec3 objectPos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::mat4 objectModel = glm::mat4(1.0f);
    objectModel = glm::translate(objectModel, objectPos);

    lightShader.use();
    glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(lightModel));
    glUniform4f(glGetUniformLocation(lightShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    shaderProgram.use();
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(objectModel));
    glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

    // Habilitar pruebas de profundidad y renderizado de caras ocultas
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CCW);

    Camera camera(SCR_WIDTH, SCR_HEIGHT, glm::vec3(0.0f, 0.0f, 3.0f));

    double prevTime = 0.0;
    double crntTime = 0.0;
    double timeDiff;
    unsigned int counter = 0;
    glfwSwapInterval(0);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Bucle de renderizado
    while (!glfwWindowShouldClose(window)) {
        // Calcular el tiempo transcurrido
        crntTime = glfwGetTime();
        timeDiff = crntTime - prevTime;
        counter++;

        if (timeDiff >= 1.0 / 100.0) {
            string FPS = to_string((1.0 / timeDiff) * counter);
            string ms = to_string((timeDiff / counter) * 1000);
            string newTitle = "Voxel_Game_Engine - " + FPS + "FPS / " + ms + "ms";
            glfwSetWindowTitle(window, newTitle.c_str());
            prevTime = crntTime;
            counter = 0;
            if (!io.WantCaptureMouse) {
                camera.Inputs(window);
            }
        }

		glEnable(GL_DEPTH_TEST);

        // Procesar entrada
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // Renderizar
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Start the ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Procesar entrada de la c�mara
        camera.UpdateMatrix(45.0f, 0.1f, 100.0f);

        // draw the meshes
        //Cube.Draw(shaderProgram, camera);
        light.Draw(lightShader, camera);
        chunk.render(shaderProgram, camera);

        ImGui::Begin("Light Settings");
        ImGui::Text("Test");
        ImGui::Text("Camera Position: (%f, %f, %f)", camera.Position.x, camera.Position.y, camera.Position.z);
        ImGui::Text("Light Position: (%f, %f, %f)", lightPos.x, lightPos.y, lightPos.z);
        ImGui::Text("Light Intensity: %f", lightIntensity);
        ImGui::Text("Cube Scale: %f", cubeScale);
        ImGui::End();

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Intercambiar buffers y procesar eventos
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Eliminar recursos
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    shaderProgram.Delete();
    lightShader.Delete();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}