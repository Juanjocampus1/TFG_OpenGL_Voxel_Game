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
#include "chunk.h"
#include "vao.h"
#include "vbo.h"
#include "ebo.h"

using namespace std;

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

GLfloat lightvertices[] = {
    // Posiciones (x, y, z)
    -0.1f, -0.1f,  0.1f,  // Vértice 0 (inferior izquierda frontal)
    -0.1f, -0.1f, -0.1f,  // Vértice 1 (inferior izquierda trasera)
     0.1f, -0.1f, -0.1f,  // Vértice 2 (inferior derecha trasera)
     0.1f, -0.1f,  0.1f,  // Vértice 3 (inferior derecha frontal)
    -0.1f,  0.1f,  0.1f,  // Vértice 4 (superior izquierda frontal)
    -0.1f,  0.1f, -0.1f,  // Vértice 5 (superior izquierda trasera)
     0.1f,  0.1f, -0.1f,  // Vértice 6 (superior derecha trasera)
     0.1f,  0.1f,  0.1f   // Vértice 7 (superior derecha frontal)
};

GLuint lightIndices[] = {
    0, 1, 2, 0, 2, 3,   // Cara inferior
    4, 5, 6, 4, 6, 7,   // Cara superior
    0, 4, 5, 0, 5, 1,   // Cara izquierda
    2, 3, 7, 2, 7, 6,   // Cara derecha
    1, 5, 6, 1, 6, 2,   // Cara trasera
    0, 3, 7, 0, 7, 4    // Cara delantera
};

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

    // Cargar y compilar shaders
    Shader shaderProgram("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");
    Shader lightShader("shaders/light_vertex_shader.glsl", "shaders/light_fragment_shader.glsl");

    // Crear el chunk y sus buffers
    Chunk chunk(16, 10, 16);

    VAO lightVAO;
    lightVAO.Bind();

    VBO lightVBO(lightvertices, sizeof(lightvertices));
    EBO lightEBO(lightIndices, sizeof(lightIndices));

    // Atributo de posición
    lightVAO.LinkVBO(lightVBO, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);

    // Desvincular VAO, VBO y EBO
    lightVAO.Unbind();
    lightVBO.Unbind();
    lightEBO.Unbind();

    glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    // Configuración de la posición y transformación del cubo de luz
    glm::vec3 lightPos(-2.0f, 15.0f, -2.0f); // Posición del cubo de luz, ajusta según sea necesario
    glm::mat4 lightModel = glm::mat4(1.0f);
    lightModel = glm::translate(lightModel, lightPos);

    glm::mat4 chunkModel = glm::mat4(1.0f);
    chunkModel = glm::translate(chunkModel, glm::vec3(0.0f, 0.0f, 0.0f)); // Ajusta la posición según sea necesario

    lightShader.use();
    glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(lightModel));
    glUniform4f(glGetUniformLocation(lightShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    shaderProgram.use();
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(chunkModel));
    glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

    // Cargar texturas
    Texture texture("Texture_atlas.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
    texture.texUnit(shaderProgram, "texture1", 0);
    Texture textureSpec("Texture_atlas_spec.png", GL_TEXTURE_2D, 1, GL_RED, GL_UNSIGNED_BYTE);
    textureSpec.texUnit(shaderProgram, "texture3", 1);

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
            camera.Inputs(window);
        }

        // Procesar entrada
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // Renderizar
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Procesar entrada de la cámara
        camera.UpdateMatrix(45.0f, 0.1f, 100.0f);

        // Renderizar el chunk
        shaderProgram.use();
        texture.Bind();
        textureSpec.Bind();
        glUniform3f(glGetUniformLocation(shaderProgram.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
        camera.Matrix(shaderProgram, "camMatrix");

        chunk.render(shaderProgram, texture, camera);

        // Renderizar el cubo de luz
        lightShader.use();
        camera.Matrix(lightShader, "camMatrix");
        lightVAO.Bind();
        glDrawElements(GL_TRIANGLES, sizeof(lightIndices) / sizeof(int), GL_UNSIGNED_INT, 0);

        // Intercambiar buffers y procesar eventos
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Liberar recursos
    lightVAO.Delete();
    lightVBO.Delete();
    lightEBO.Delete();
    texture.Delete();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
