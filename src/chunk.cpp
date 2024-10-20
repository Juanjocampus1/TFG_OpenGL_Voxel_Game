#include "Header_files/Chunk.h"


// Constructor de Chunk
Chunk::Chunk(int width, int height, int depth, std::vector<Vertex>& vertices, std::vector<GLuint>& indices, std::vector<Texture>& textures)
    : width(width), height(height), depth(depth),
    chunkData(width, std::vector<std::vector<bool>>(height, std::vector<bool>(depth, false))),
    mesh(vertices, indices, textures)
{
    generateChunk();
    setupMesh();
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

// Genera los datos del chunk
void Chunk::generateChunk() {
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            for (int z = 0; z < depth; ++z) {
                // Puedes añadir lógica específica para decidir dónde generar cubos
                chunkData[x][y][z] = true;
            }
        }
    }
}

// Nueva función para configurar la malla
void Chunk::setupMesh() {
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture> textures;

    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            for (int z = 0; z < depth; ++z) {
                if (chunkData[x][y][z]) {
                    addCube(vertices, indices, x, y, z);
                }
            }
        }
    }

    mesh = Mesh(vertices, indices, textures);
}


// Función para añadir un cubo a los vértices e índices
void Chunk::addCube(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, int x, int y, int z) {
    GLuint startIndex = static_cast<GLuint>(vertices.size());  // Indice inicial para los vértices del cubo

    // Vértices del cubo (usando las posiciones relativas a x, y, z)
    Vertex cubeVertices[] = {
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

    // Añade los vértices al vector de vértices
    vertices.insert(vertices.end(), std::begin(cubeVertices), std::end(cubeVertices));

    // Añadir índices para las caras visibles
    GLuint cubeIndices[] = {
        0, 1, 2, 0, 2, 3,   // Cara trasera
        4, 7, 6, 4, 6, 5,   // Cara delantera (ajustada)
        8, 11, 10, 8, 10, 9,   // Cara izquierda (ajustada)
        12, 13, 14, 12, 14, 15,   // Cara derecha
        16, 19, 18, 16, 18, 17,   // Cara inferior (ajustada)
        20, 21, 22, 20, 22, 23    // Cara superior
    };

    for (int i = 0; i < sizeof(cubeIndices) / sizeof(GLuint); ++i) {
        indices.push_back(startIndex + cubeIndices[i]);  // Ajusta los índices según los nuevos vértices
    }
}

// Función de renderizado usando Mesh
void Chunk::render(Shader& shader, Camera& camera) {
    shader.use();
    camera.Matrix(shader, "camMatrix");

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
                            mesh.Draw(shader, camera);// Usamos la clase Mesh para el dibujo
                        }
                    }
                }
            }
        }
    }
}
