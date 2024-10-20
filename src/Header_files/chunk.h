#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"

class Chunk {
public:
    //Chunk(int width, int height, int depth);
    Chunk(int width, int height, int depth, std::vector<Vertex>& vertices, std::vector<GLuint>& indices, std::vector<Texture>& textures);

    // Función para renderizar el chunk usando el shader y la cámara
    void render(Shader& shader, Camera& camera);

private:
    int width, height, depth;

    // Datos del chunk: define qué partes del chunk tienen un cubo
    std::vector<std::vector<std::vector<bool>>> chunkData;

    // Malla para almacenar los vértices e índices
    Mesh mesh;

    // Funciones privadas
    void generateChunk();  // Genera la estructura del chunk
    void setupMesh();// Configura la malla generando vértices e índices
    bool isFaceVisible(int x, int y, int z, int face);
    bool isCubeAt(int x, int y, int z);

    // Añade un cubo en las posiciones dadas (x, y, z)
    void addCube(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, int x, int y, int z);
};
