#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Header_files/Mesh.h"
#include "Header_files/Chunk.h"
#include "Header_files/Planet.h"

using namespace std;

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

int chunkSize = 16;
int prevChunkSize = chunkSize;

// Función de callback para ajustar el tamaño del viewport
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main() {
    // Inicialización de GLFW
    if (!glfwInit()) {
        cout << "Falló la inicialización de GLFW" << endl;
        return -1;
    }

    // Configuración de la versión de OpenGL y perfil
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Creación de la ventana
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Voxel Game Engine", NULL, NULL);
    if (window == NULL) {
        cout << "Falló la creación de la ventana GLFW" << endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cout << "Falló la inicialización de GLAD" << endl;
        return -1;
    }

    // Ajuste del tamaño del viewport
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    std::vector<Texture> textures = {
        Texture("Texture_atlas.png", "diffuse", 0, GL_RGBA, GL_UNSIGNED_BYTE),
        Texture("Texture_atlas_specular.png", "specular", 1, GL_RED, GL_UNSIGNED_BYTE)
    };

    Shader shaderProgram("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");
	//Chunk chunk(chunkSize, glm::vec3(0.0f, 0.0f, 0.0f));

    Planet planet;

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

        // Procesar entrada de la cámara
        camera.UpdateMatrix(45.0f, 0.1f, 100.0f);

        /*
        if (chunkSize != prevChunkSize) {
            cout << "Cambiando el tamaño del chunk a: " << chunkSize << endl;
            chunk = Chunk(chunkSize, glm::vec3(0.0f, 0.0f, 0.0f));
            prevChunkSize = chunkSize;
        }
        */

        //chunk.Render(shaderProgram, camera);

		planet.GetChunkData(camera.Position.x, camera.Position.y, camera.Position.z);

        ImGui::Begin("Light Settings");
        ImGui::Text("Test");
        ImGui::Text("Camera Position: (%f, %f, %f)", camera.Position.x, camera.Position.y, camera.Position.z);
        //ImGui::Text("Cube Count: %u", chunk.GetCubeCount());
        //ImGui::Text("chunks generated: %u", chunk.GetChunkCount());
        ImGui::SliderInt("Chunk Size", &chunkSize, 1, 64);
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
    //chunk.~Chunk();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}