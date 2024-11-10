#include "Header_files/Planet.h"
/*
Planet::Planet(std::vector<Texture> textures) {
    // Generar chunks iniciales
    for (int x = -renderDistance; x <= renderDistance; ++x) {
        for (int y = -renderDistance; y <= renderDistance; ++y) {
            for (int z = -renderDistance; z <= renderDistance; ++z) {
                std::string chunkKey = std::to_string(x) + "_" + std::to_string(y) + "_" + std::to_string(z);
                chunks[chunkKey] = Chunk(chunkSize, glm::vec3(x * chunkSize, y * chunkSize, z * chunkSize), textures);
            }
        }
    }
}

Planet::~Planet() {
    // Liberar memoria de los chunks
    for (auto& chunk : chunks) {
        delete chunk.second.mesh;
    }
}

std::vector<unsigned int> Planet::GetChunkData(int chunkX, int chunkY, int chunkZ) {
    std::string chunkKey = std::to_string(chunkX) + "_" + std::to_string(chunkY) + "_" + std::to_string(chunkZ);
    if (chunks.find(chunkKey) != chunks.end()) {
        return chunks[chunkKey].chunkData;
    }
    return std::vector<unsigned int>();
}

void Planet::Update(float camX, float camY, float camZ, Shader& shader, Camera& camera, std::vector<Texture> textures) {
    // Actualizar chunks según la posición de la cámara
    int camChunkX = static_cast<int>(camX) / chunkSize;
    int camChunkY = static_cast<int>(camY) / chunkSize;
    int camChunkZ = static_cast<int>(camZ) / chunkSize;

    for (int x = camChunkX - renderDistance; x <= camChunkX + renderDistance; ++x) {
        for (int y = camChunkY - renderDistance; y <= camChunkY + renderDistance; ++y) {
            for (int z = camChunkZ - renderDistance; z <= camChunkZ + renderDistance; ++z) {
                std::string chunkKey = std::to_string(x) + "_" + std::to_string(y) + "_" + std::to_string(z);
                if (chunks.find(chunkKey) == chunks.end()) {
                    chunks[chunkKey] = Chunk(chunkSize, glm::vec3(x * chunkSize, y * chunkSize, z * chunkSize), textures);
                }
            }
        }
    }

    // Renderizar los chunks
    for (auto& chunk : chunks) {
        chunk.second.Render(shader, camera);
    }
}

std::vector<unsigned int> Planet::GenerateChunkData(int chunkX, int chunkY, int chunkZ) {
    // Generar datos del chunk (esto puede ser personalizado según la lógica del juego)
    std::vector<unsigned int> chunkData(chunkSize * chunkSize * chunkSize, 1); // Ejemplo: llenar el chunk con bloques sólidos
    return chunkData;
}
*/