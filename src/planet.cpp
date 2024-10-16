// planeta.cpp
#include "planet.h"
#include <cmath>
#include <iostream>
#include <set> // Incluir el encabezado <set>

Planeta::Planeta(int renderDistance) : renderDistance(renderDistance) {}

void Planeta::generateWorld(int chunkWidth, int chunkHeight, int chunkDepth) {
    // Generar chunks iniciales alrededor del origen (0, 0, 0)
    for (int x = -renderDistance; x <= renderDistance; ++x) {
        for (int z = -renderDistance; z <= renderDistance; ++z) {
            if (std::sqrt(x * x + z * z) <= renderDistance) {
                glm::vec3 chunkPos(x * chunkWidth, 0, z * chunkDepth);
                chunks.emplace_back(chunkWidth, chunkHeight, chunkDepth);
                chunkPositions.push_back(chunkPos);
            }
        }
    }
}

void Planeta::renderWorld(Shader& shader, Texture& texture, Camera& camera, glm::vec3 playerPosition) {
    updateChunks(playerPosition);
    for (size_t i = 0; i < chunks.size(); ++i) {
        if (isChunkInRenderDistance(chunkPositions[i], playerPosition)) {
            chunks[i].render(shader, texture, camera);
        }
    }
}

void Planeta::updateChunks(glm::vec3 playerPosition) {
    int chunkWidth = chunks[0].getWidth();
    int chunkDepth = chunks[0].getDepth();
	int chunkHeight = chunks[0].getHeight();

    // Calcular la posición del chunk en el que se encuentra el jugador
    int playerChunkX = static_cast<int>(std::floor(playerPosition.x / chunkWidth));
    int playerChunkZ = static_cast<int>(std::floor(playerPosition.z / chunkDepth));

    // Crear un conjunto de posiciones de chunks que deberían estar presentes
    std::set<std::pair<int, int>> requiredChunks;
    for (int x = -renderDistance; x <= renderDistance; ++x) {
        for (int z = -renderDistance; z <= renderDistance; ++z) {
            if (std::sqrt(x * x + z * z) <= renderDistance) {
                requiredChunks.emplace(playerChunkX + x, playerChunkZ + z);
            }
        }
    }

    // Crear un conjunto de posiciones de chunks que actualmente están presentes
    std::set<std::pair<int, int>> currentChunks;
    for (const auto& pos : chunkPositions) {
        int chunkX = static_cast<int>(std::floor(pos.x / chunkWidth));
        int chunkZ = static_cast<int>(std::floor(pos.z / chunkDepth));
        currentChunks.emplace(chunkX, chunkZ);
    }

    // Determinar los chunks que deben ser eliminados
    std::vector<size_t> chunksToRemove;
    for (size_t i = 0; i < chunkPositions.size(); ++i) {
        int chunkX = static_cast<int>(std::floor(chunkPositions[i].x / chunkWidth));
        int chunkZ = static_cast<int>(std::floor(chunkPositions[i].z / chunkDepth));
        if (requiredChunks.find({ chunkX, chunkZ }) == requiredChunks.end()) {
            chunksToRemove.push_back(i);
        }
    }

    // Eliminar los chunks que están fuera del rango de renderizado
    for (auto it = chunksToRemove.rbegin(); it != chunksToRemove.rend(); ++it) {
        chunks.erase(chunks.begin() + *it);
        chunkPositions.erase(chunkPositions.begin() + *it);
    }

    // Determinar los chunks que deben ser agregados
    for (const auto& pos : requiredChunks) {
        if (currentChunks.find(pos) == currentChunks.end()) {
            int chunkX = pos.first * chunkWidth;
            int chunkZ = pos.second * chunkDepth;
            glm::vec3 chunkPos(chunkX, 0, chunkZ);
            chunks.emplace_back(chunkWidth, chunkHeight, chunkDepth);
            chunkPositions.push_back(chunkPos);
        }
    }
}


bool Planeta::isChunkInRenderDistance(glm::vec3 chunkPosition, glm::vec3 playerPosition) {
    float distance = glm::distance(glm::vec3(chunkPosition.x, 0, chunkPosition.z), glm::vec3(playerPosition.x, 0, playerPosition.z));
    return distance <= renderDistance * chunks[0].getWidth(); // Usar el método getWidth
}
