#ifndef CHUNK_H
#define CHUNK_H

#include "Block.h"
#include "../Renderer/VAO.h"
#include "../Renderer/VBO.h"
#include "../Renderer/EBO.h"
#include <vector>
#include <glm/glm.hpp>
#include <map>
#include <tuple>
#include "WorldGen.h"

class Chunk {
    public:
        static const int SIZE = 16; // reduced size for performance
            
        Block blocks[SIZE][SIZE][SIZE];
        glm::vec3 position;
        bool meshBuilt = false;
        bool meshDirty = true;
        int dirtyDelay = 0; // frames to wait before rebuilding mesh
        int chunkX, chunkY, chunkZ;
        WorldGen* worldGen;
            
        Chunk(int x, int y, int z, WorldGen* wg);
        ~Chunk();
        void generate();
        void setWorldGen(WorldGen* wg);
        void rebuildMesh(const std::map<std::tuple<int, int, int>, Chunk*>& allChunks);
        void draw(class Shader& shader, class Camera& camera);
            
        // Accessors for batch renderer
        const std::vector<char>& getRawVertices() const { return rawVertices; }
        const std::vector<unsigned int>& getIndices() const { return indices; }
        bool hasMeshBuilt() const { return meshBuilt; }
        void setDirtyDelay(int frames) { dirtyDelay = frames; meshDirty = true; }
        int getDirtyDelay() const { return dirtyDelay; }
            
    private:
        VAO* vao;
        VBO* vbo;
        EBO* ebo;
        std::vector<char> rawVertices; // store vertex bytes to save memory
        std::vector<unsigned int> indices;
};

#endif