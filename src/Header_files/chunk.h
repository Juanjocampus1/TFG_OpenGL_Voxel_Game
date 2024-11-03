#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include "Camera.h"
#include "Shader.h"

struct ChunkVertex {

    glm::vec3 position;
    glm::vec3 color;
    glm::vec3 normal;
    glm::vec2 texCoords;

    ChunkVertex(glm::vec3 pos, glm::vec3 col, glm::vec3 norm, glm::vec2 tex)
        : position(pos), color(col), normal(norm), texCoords(tex) {}
};

class Chunk {
    public:
        Chunk(unsigned int chunkSize, glm::vec3 chunkPos);
		~Chunk();

		void GenerateChunk();
        void Render(Shader& shader);
        int GetCubeCount() const;
        static int GetChunkCount();

    private:
        unsigned int chunkSize;
        unsigned int numTriangles;
        std::vector<ChunkVertex> vertices;
        std::vector<unsigned int> indices;
        unsigned int vao, vbo, ebo;
        glm::vec3 chunkPos;
        static int chunkCount;
};
