// chunk.cpp
#include "chunk.h"
#include <iostream>

// Cubo de referencia (el mismo que el usado en main)
// Vertices y colores del cubo
GLfloat cubeVertices[] = {
    // posiciones          // colores      // coordenadas de textura (U, V)      //normals
    // Cara trasera
    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  16.0f / 32.0f, 0.0f / 32.0f,             0.0f, 0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  32.0f / 32.0f, 0.0f / 32.0f, 	          0.0f, 0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  32.0f / 32.0f, 16.0f / 32.0f,            0.0f, 0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  16.0f / 32.0f, 16.0f / 32.0f,            0.0f, 0.0f, -1.0f,

    // Cara delantera
    -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,  16.0f / 32.0f, 0.0f / 32.0f,             0.0f, 0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,  32.0f / 32.0f, 0.0f / 32.0f,             0.0f, 0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  32.0f / 32.0f, 16.0f / 32.0f,            0.0f, 0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.0f,  16.0f / 32.0f, 16.0f / 32.0f,            0.0f, 0.0f, 1.0f,

    // Cara izquierda
    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  16.0f / 32.0f, 0.0f / 32.0f,             -1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,  32.0f / 32.0f, 0.0f / 32.0f,             -1.0f, 0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.0f,  32.0f / 32.0f, 16.0f / 32.0f,            -1.0f, 0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  16.0f / 32.0f, 16.0f / 32.0f,            -1.0f, 0.0f, 0.0f,

    // Cara derecha
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  16.0f / 32.0f, 0.0f / 32.0f,             1.0f, 0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,  32.0f / 32.0f, 0.0f / 32.0f,             1.0f, 0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  32.0f / 32.0f, 16.0f / 32.0f,            1.0f, 0.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  16.0f / 32.0f, 16.0f / 32.0f,            1.0f, 0.0f, 0.0f,

     // Cara inferior
     -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  0.0f / 32.0f, 1.0f - 32.0f / 32.0f,     0.0f, -1.0f, 0.0f,
      0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  16.0f / 32.0f, 1.0f - 32.0f / 32.0f,    0.0f, -1.0f, 0.0f,
      0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,  16.0f / 32.0f, 1.0f - 16.0f / 32.0f,    0.0f, -1.0f, 0.0f,
     -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,  0.0f / 32.0f, 1.0f - 16.0f / 32.0f,     0.0f, -1.0f, 0.0f,

     // Cara superior
     -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  16.0f / 32.0f, 1.0f - 0.0f / 32.0f,     0.0f, 1.0f, 0.0f,
      0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  32.0f / 32.0f, 1.0f - 0.0f / 32.0f,     0.0f, 1.0f, 0.0f,
      0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,  32.0f / 32.0f, 1.0f - 16.0f / 32.0f,    0.0f, 1.0f, 0.0f,
     -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  16.0f / 32.0f, 1.0f - 16.0f / 32.0f,    0.0f, 1.0f, 0.0f
};

GLuint cubeIndices[] = {
    0, 1, 2, 0, 2, 3,   // Cara trasera
    4, 7, 6, 4, 6, 5,   // Cara delantera (ajustada)
    8, 11, 10, 8, 10, 9,   // Cara izquierda (ajustada)
    12, 13, 14, 12, 14, 15,   // Cara derecha
    16, 19, 18, 16, 18, 17,   // Cara inferior (ajustada)
    20, 21, 22, 20, 22, 23    // Cara superior
};

// Definición de las caras
enum Face { BACK, FRONT, LEFT, RIGHT, BOTTOM, TOP };

Chunk::Chunk(int width, int height, int depth)
    : width(width), height(height), depth(depth), vbo(nullptr, 0), ebo(nullptr, 0), chunkData(width, std::vector<std::vector<bool>>(height, std::vector<bool>(depth, false))) {
    generateChunk();
    setupBuffers();
}

bool Chunk::isCubeAt(int x, int y, int z) {
    // Verifica si la posición está dentro de los límites del chunk
    if (x < 0 || x >= width || y < 0 || y >= height || z < 0 || z >= depth) {
        return false; // Si está fuera de los límites, no hay cubo
    }
    // Retorna si hay un cubo en la posición (x, y, z)
    return chunkData[x][y][z];
}

bool Chunk::isFaceVisible(int x, int y, int z, int face) {
    // Verifica si la posición está fuera de los límites del chunk
    if (x < 0 || x >= width || y < 0 || y >= height || z < 0 || z >= depth) {
        return true; // Si está fuera de los límites, la cara es visible
    }

    // Verifica si hay un cubo en la posición adyacente
    switch (face) {
    case 0: return !isCubeAt(x, y, z - 1); // Cara trasera
    case 1: return !isCubeAt(x, y, z + 1); // Cara delantera
    case 2: return !isCubeAt(x - 1, y, z); // Cara izquierda
    case 3: return !isCubeAt(x + 1, y, z); // Cara derecha
    case 4: return !isCubeAt(x, y - 1, z); // Cara inferior
    case 5: return !isCubeAt(x, y + 1, z); // Cara superior
    default: return false;
    }
}

void Chunk::generateChunk() {
    // Aquí puedes inicializar chunkData con cubos en las posiciones deseadas
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            for (int z = 0; z < depth; ++z) {
                // Por ejemplo, llena el chunk con cubos en todas las posiciones
                chunkData[x][y][z] = true;
            }
        }
    }
}

void Chunk::setupBuffers() {
    // Configura los buffers VAO, VBO y EBO
    vao.Bind();
    vbo.Bind();
    ebo.Bind();

    // Cargar datos en el VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    // Cargar datos en el EBO
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);


    vao.LinkVBO(vbo, 0, 3, GL_FLOAT, 11 * sizeof(float), (void*)0);
    vao.LinkVBO(vbo, 1, 3, GL_FLOAT, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    vao.LinkVBO(vbo, 2, 2, GL_FLOAT, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    vao.LinkVBO(vbo, 3, 3, GL_FLOAT, 11 * sizeof(float), (void*)(8 * sizeof(float)));

    vao.Unbind();
    vbo.Unbind();
    ebo.Unbind();
}

void Chunk::render(Shader& shader, Texture& texture, Camera& camera) {
    shader.use();
    texture.Bind();
    camera.Matrix(shader, "camMatrix");

    vao.Bind();

    // Recorre el chunk
    for (int x = 0; x < width; ++x) {
        // Invertir el bucle 'y' para que vaya desde la altura máxima hasta 0
        for (int y = 0; y < height; ++y) {
            for (int z = 0; z < depth; ++z) {
                if (chunkData[x][y][z]) { // Si hay un cubo en la posición actual

                    // Aplica la matriz de modelo para posicionar el cubo
                    glm::mat4 model = glm::mat4(1.0f);
                    model = glm::translate(model, glm::vec3(x, y, z));
                    shader.setMat4("model", model);  // Pasa la matriz de modelo al shader

                    // Renderiza solo las caras visibles
                    for (int face = 0; face < 6; ++face) {
                        if (isFaceVisible(x, y, z, face)) {
                            // Renderiza la cara correspondiente usando los índices adecuados
                            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(face * 6 * sizeof(GLuint)));
                        }
                    }
                }
            }
        }
    }
    vao.Unbind();
}

int Chunk::getWidth() const {
	return width;
}

int Chunk::getHeight() const {
	return height;
}

int Chunk::getDepth() const {
	return depth;
}