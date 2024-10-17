#include "Mesh.h"

using namespace std;

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

Vertex vertices[] = {
    // posiciones                            // colores              // coordenadas de textura (U, V)             //normals
    // Cara trasera
    {glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(16.0f / 32.0f, 0.0f / 32.0f), glm::vec3(0.0f, 0.0f, -1.0f)},
    {glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(32.0f / 32.0f, 0.0f / 32.0f), glm::vec3(0.0f, 0.0f, -1.0f)},
    {glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(32.0f / 32.0f, 16.0f / 32.0f), glm::vec3(0.0f, 0.0f, -1.0f)},
    {glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(16.0f / 32.0f, 16.0f / 32.0f), glm::vec3(0.0f, 0.0f, -1.0f)},

    // Cara delantera
    {glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec2(16.0f / 32.0f, 0.0f / 32.0f), glm::vec3(0.0f, 0.0f, 1.0f)},
    {glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.0f, 1.0f, 1.0f), glm::vec2(32.0f / 32.0f, 0.0f / 32.0f), glm::vec3(0.0f, 0.0f, 1.0f)},
    {glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(32.0f / 32.0f, 16.0f / 32.0f), glm::vec3(0.0f, 0.0f, 1.0f)},
    {glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(16.0f / 32.0f, 16.0f / 32.0f), glm::vec3(0.0f, 0.0f, 1.0f)},

    // Cara izquierda
    {glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(16.0f / 32.0f, 0.0f / 32.0f), glm::vec3(-1.0f, 0.0f, 0.0f)},
    {glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec2(32.0f / 32.0f, 0.0f / 32.0f), glm::vec3(-1.0f, 0.0f, 0.0f)},
    {glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(32.0f / 32.0f, 16.0f / 32.0f), glm::vec3(-1.0f, 0.0f, 0.0f)},
    {glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(16.0f / 32.0f, 16.0f / 32.0f), glm::vec3(-1.0f, 0.0f, 0.0f)},

    // Cara derecha
    {glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(16.0f / 32.0f, 0.0f / 32.0f), glm::vec3(1.0f, 0.0f, 0.0f)},
    {glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.0f, 1.0f, 1.0f), glm::vec2(32.0f / 32.0f, 0.0f / 32.0f), glm::vec3(1.0f, 0.0f, 0.0f)},
    {glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(32.0f / 32.0f, 16.0f / 32.0f), glm::vec3(1.0f, 0.0f, 0.0f)},
    {glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(16.0f / 32.0f, 16.0f / 32.0f), glm::vec3(1.0f, 0.0f, 0.0f)},

	// Cara inferior
	{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f / 32.0f, 1.0f - 32.0f / 32.0f), glm::vec3(0.0f, -1.0f, 0.0f)},
	{glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(16.0f / 32.0f, 1.0f - 32.0f / 32.0f), glm::vec3(0.0f, -1.0f, 0.0f)},
	{glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(16.0f / 32.0f, 1.0f - 32.0f / 32.0f), glm::vec3(0.0f, -1.0f, 0.0f)},
	{glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(0.0f / 32.0f, 1.0f - 32.0f / 32.0f), glm::vec3(0.0f, -1.0f, 0.0f)},

	// Cara superior
	{glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(16.0f / 32.0f, 1.0f - 0.0f / 32.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
	{glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(32.0f / 32.0f, 1.0f - 0.0f / 32.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
	{glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(32.0f / 32.0f, 1.0f - 16.0f / 32.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
	{glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(16.0f / 32.0f, 1.0f - 16.0f / 32.0f), glm::vec3(0.0f, 1.0f, 0.0f)}
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


	Texture textures[]
	{
		Texture("Texture_atlas.png", "diffuse", 0, GL_RGBA, GL_UNSIGNED_BYTE),
		Texture("Texture_atlas_specular.png", "specular", 1, GL_RED, GL_UNSIGNED_BYTE)
	};

    // draw a cube
    Shader shaderProgram("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");
	vector <Vertex> verts(vertices, vertices + sizeof(vertices) / sizeof(Vertex));
	vector <GLuint> ind(indices, indices + sizeof(indices) / sizeof(GLuint));
	vector <Texture> tex(textures, textures + sizeof(textures) / sizeof(Texture));
	Mesh Cube(verts, ind, tex);

	//draw a light cube
    Shader lightShader("shaders/light_vertex_shader.glsl", "shaders/light_fragment_shader.glsl");
	vector <Vertex> lightVerts(lightVertices, lightVertices + sizeof(lightVertices) / sizeof(Vertex));
	vector <GLuint> lightInd(lightIndices, lightIndices + sizeof(lightIndices) / sizeof(GLuint));
	Mesh light(lightVerts, lightInd, tex);

    glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec3 lightPos = glm::vec3(1.0f, 1.0f, 1.0f);
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

		// draw the meshes
		Cube.Draw(shaderProgram, camera);
		light.Draw(lightShader, camera);

        // Intercambiar buffers y procesar eventos
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

	// Eliminar recursos
	shaderProgram.Delete();
	lightShader.Delete();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}