#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "texture.h"
#include "shader.h"
#include "camera.h"
#include "vao.h"
#include "vbo.h"
#include "ebo.h"

using namespace std;

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

GLfloat lightvertices[] = {
    // Posiciones
    -0.1f, -0.1f,  0.1f,
    -0.1f, -0.1f, -0.1f,
     0.1f, -0.1f, -0.1f,
     0.1f, -0.1f,  0.1f,
    -0.1f,  0.1f,  0.1f,
    -0.1f,  0.1f, -0.1f,
     0.1f,  0.1f, -0.1f,
     0.1f,  0.1f,  0.1f
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

// Vertices y colores del cubo
GLfloat vertices[] = {
    // posiciones          // colores           // coordenadas de textura (U, V)
    // Cara trasera
    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  16.0f / 32.0f, 0.0f / 32.0f, // Abajo-izquierda (Césped y Tierra)
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  32.0f / 32.0f, 0.0f / 32.0f, // Abajo-derecha
     0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  32.0f / 32.0f, 16.0f / 32.0f,  // Arriba-derecha (Césped puro)
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  16.0f / 32.0f, 16.0f / 32.0f,  // Arriba-izquierda

    // Cara delantera
    -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,  16.0f / 32.0f, 0.0f / 32.0f, // Abajo-izquierda (Césped y Tierra)
     0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,  32.0f / 32.0f, 0.0f / 32.0f, // Abajo-derecha
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  32.0f / 32.0f, 16.0f / 32.0f,  // Arriba-derecha (Césped puro)
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.0f,  16.0f / 32.0f, 16.0f / 32.0f,  // Arriba-izquierda

    // Cara izquierda
    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  16.0f / 32.0f, 0.0f / 32.0f, // Abajo-derecha (Césped puro)
    -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,  32.0f / 32.0f, 0.0f / 32.0f,  // Abajo-izquierda
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.0f,  32.0f / 32.0f, 16.0f / 32.0f, // Arriba-izquierda (Césped y Tierra)
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  16.0f / 32.0f, 16.0f / 32.0f, // Arriba-derecha

    // Cara derecha
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  16.0f / 32.0f, 0.0f / 32.0f, // Abajo-derecha (Césped puro)
     0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,  32.0f / 32.0f, 0.0f / 32.0f,  // Abajo-izquierda
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  32.0f / 32.0f, 16.0f / 32.0f, // Arriba-izquierda (Césped y Tierra)
     0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  16.0f / 32.0f, 16.0f / 32.0f, // Arriba-derecha

     // Cara inferior
     -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  0.0f / 32.0f, 1.0f - 32.0f / 32.0f, // Abajo-izquierda (Tierra)
      0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  16.0f / 32.0f, 1.0f - 32.0f / 32.0f, // Abajo-derecha
      0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,  16.0f / 32.0f, 1.0f - 16.0f / 32.0f, // Arriba-derecha (Césped y Tierra)
     -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,  0.0f / 32.0f, 1.0f - 16.0f / 32.0f, // Arriba-izquierda

     // Cara superior
     -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  16.0f / 32.0f, 1.0f - 0.0f / 32.0f,  // Abajo-derecha (Césped puro)
      0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  32.0f / 32.0f, 1.0f - 0.0f / 32.0f,  // Abajo-derecha (Césped puro)
      0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,  32.0f / 32.0f, 1.0f - 16.0f / 32.0f, // Arriba-derecha (Césped y Tierra)
     -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  16.0f / 32.0f, 1.0f - 16.0f / 32.0f, // Arriba-izquierda
};

GLuint indices[] = {
    0, 1, 2, 0, 2, 3, // Cara trasera
    4, 5, 6, 4, 6, 7, // Cara delantera
    8, 9, 10, 8, 10, 11, // Cara izquierda
    12, 13, 14, 12, 14, 15, // Cara derecha
    16, 17, 18, 16, 18, 19, // Cara inferior
    20, 21, 22, 20, 22, 23  // Cara superior
};

// Callback para ajustar el viewport cuando se cambia el tamaño de la ventana
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main() {

    // Initialize GLFW
    if (!glfwInit()) {
        cout << "Failed to initialize GLFW" << endl;
        return -1;
    }

    // Tell GLFW what version of OpenGL we are using 
    // In this case we are using OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // Tell GLFW we are using the CORE profile
    // So that means we only have the modern functions
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a GLFWwindow object of 800 by 800 pixels, naming it "OpenGL_Learn"
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Voxel_Game_Engine", NULL, NULL);
    if (window == NULL) {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }

    // Introduce the window into the current context
    glfwMakeContextCurrent(window);

    // Load GLAD so it configures OpenGL
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    // Register the callback function to adjust the viewport
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Specify the viewport of OpenGL in the Window
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    // Load and compile shaders
    Shader shaderProgram("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");

    // Crear VAO, VBO y EBO usando las clases
    VAO vao;
    vao.Bind();

    VBO vbo(vertices, sizeof(vertices));
    EBO ebo(indices, sizeof(indices));

    // Atributo de posición
    vao.LinkVBO(vbo, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Atributo de color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Atributo de coordenadas de textura
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Desvincular VAO, VBO y EBO
    vao.Unbind();
    vbo.Unbind();
    ebo.Unbind();

    Shader lightShader("shaders/light_vertex_shader.glsl", "shaders/light_fragment_shader.glsl");

    VAO lightVAO;
    lightVAO.Bind();

    VBO lightVBO(lightvertices, sizeof(lightvertices));
    EBO lightEBO(lightIndices, sizeof(lightIndices));

    // Atributo de posición
    lightVAO.LinkVBO(lightVBO, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Desvincular VAO, VBO y EBO
    lightVAO.Unbind();
    lightVBO.Unbind();
    lightEBO.Unbind();


    // Configuración de la posición y transformación del cubo de luz
    glm::vec3 lightPos(2.0f, 0.0f, 0.0f); // Posición del cubo de luz, ajusta según sea necesario
    glm::mat4 lightModel = glm::mat4(1.0f);
    lightModel = glm::translate(lightModel, lightPos);
    lightModel = glm::scale(lightModel, glm::vec3(0.2f)); // Escala más pequeño el cubo de luz

    // Configurar la posición del cubo principal
    glm::vec3 cubePos(0.0f, 0.0f, 0.0f);
    glm::mat4 cubeModel = glm::mat4(1.0f);
    cubeModel = glm::translate(cubeModel, cubePos);

    lightShader.use();
    glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(lightModel));
    shaderProgram.use();
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(cubeModel));


    // Cargar texturas
    Texture cubeTexture("Texture_atlas.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
    cubeTexture.texUnit(shaderProgram, "tex0", 0);

    glEnable(GL_DEPTH_TEST);

    Camera camera(SCR_WIDTH, SCR_HEIGHT, glm::vec3(0.0f, 0.0f, 3.0f));
    double deltaTime = 0.0f; // Tiempo entre el frame actual y el anterior
    double lastFrame = 0.0f; // Tiempo del último frame

    while (!glfwWindowShouldClose(window)) {
        std::cout << "Inicio del bucle" << std::endl;

        // Calcular el tiempo transcurrido entre frames
        double currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Procesar entrada
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // Renderizar
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Inputs de la cámara
        camera.Inputs(window);
        camera.UpdateMatrix(45.0f, 0.1f, 100.0f);

        // Dibujar el cubo principal
        std::cout << "Dibujando cubo principal" << std::endl;
        shaderProgram.use();
        camera.Matrix(shaderProgram, "camMatrix");
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(cubeModel));
        cubeTexture.Bind();
        vao.Bind();
        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);

        // Dibujar el cubo de luz
        std::cout << "Dibujando cubo de luz" << std::endl;
        lightShader.use();
        camera.Matrix(lightShader, "camMatrix");
        glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(lightModel));
        lightVAO.Bind();
        glDrawElements(GL_TRIANGLES, sizeof(lightIndices) / sizeof(int), GL_UNSIGNED_INT, 0);

        // Intercambiar los buffers
        glfwSwapBuffers(window);

        // Poll para procesar eventos
        glfwPollEvents();

        std::cout << "Fin del bucle" << std::endl;
    }

    // Deallocate resources
    vao.Delete();
    vbo.Delete();
    ebo.Delete();
    cubeTexture.Delete();
    lightVAO.Delete();
    lightVBO.Delete();
    lightEBO.Delete();

    // Terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}