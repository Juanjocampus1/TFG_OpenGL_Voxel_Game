#pragma once

#include "Mesh.h"

class Chunk {
public:
    Chunk(unsigned int chunkSize, glm::vec3 chunkPos, std::vector<Texture>& textures);
    ~Chunk();

    void GenerateChunk(std::vector<Texture>& textures);
    void Render(Shader& shader, Camera& camera);
    int GetCubeCount() const;
    static int GetChunkCount();

public:
    std::vector<unsigned int> chunkData;
    glm::vec3 chunkPos;
    bool ready;
    bool generated;

private:
    unsigned int chunkSize;
    unsigned int numTriangles;
    unsigned int vao, vbo, ebo;
    static int chunkCount;
	Mesh* mesh;
};
