/*#include "Header_files/Planet.h"

Planet::Planet() {
	for (int x = -renderDistance; x < renderDistance; x++) {
		for (int y = -renderDistance; y < renderDistance; y++) {
			for (int z = -renderDistance; z < renderDistance; z++) {
				//Chunk chunk = Chunk(chunkSize, GenerateChunkData(x, y, z), glm::vec3(x, y, z));
				std::string chunkString = std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(z);
				chunks.try_emplace(chunkString, chunkSize, GenerateChunkData(x, y, z), glm::vec3(x, y, z));
			}
		}
	}
}

Planet::~Planet() {
}

std::vector<unsigned int> Planet::GenerateChunkData(int chunkX, int chunkY, int chunkZ) {
	std::vector<unsigned int> chunkData;
	chunkData.reserve(chunkSize * chunkSize * chunkSize);
	for (unsigned int x = 0; x < chunkSize; x++) {
		for (unsigned int y = 0; y < chunkSize; y++) {
			for (unsigned int z = 0; z < chunkSize; z++) {
				//chunkData.push_back(0); //empty block
				chunkData.push_back(1); //solid block
				//chunkData.push_back(((x + y + z) % 2 == 0) ? 1 : 0); //checkerboard patern
			}
		}
	}
	return chunkData;
}
*/