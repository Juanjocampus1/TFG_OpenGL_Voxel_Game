#pragma once

#include <vector>
#include <unordered_map>
#include <string>

#include "Chunk.h"

class Planet{
	public:
		Planet(std::vector<Texture> textures);
		~Planet();

		std::vector<unsigned int>GetChunkData(int chunkX, int chunkY, int chunkZ);
		void Update(float camX, float camY, float camZ, Shader& shader, Camera& camera, std::vector<Texture> textures);

	private:
		std::vector<unsigned int> GenerateChunkData(int chunkX, int chunkY, int chunkZ);

		std::unordered_map<std::string, Chunk> chunks;
		int renderDistance = 1;
		int chunkSize = 16;

};
