#include "Header_files/chunk.h"
#include "Header_files/Blocks.h"

int Chunk::chunkCount = 0;

Chunk::Chunk(unsigned int chunkSize, glm::vec3 chunkPos, std::vector<Texture>& textures)
	: chunkSize(chunkSize), numTriangles(0), mesh(nullptr) {
	chunkCount++;
	GenerateChunk(textures);
}

Chunk::~Chunk() {
	chunkCount--;
	delete mesh;
}

void Chunk::GenerateChunk(std::vector<Texture>& textures) {

	std::vector<Vertex> vertices;
	std::vector <GLuint> indices;

	chunkData.reserve(chunkSize * chunkSize * chunkSize);
	unsigned int currentVertices = 0;
	for (unsigned int x = 0; x < chunkSize; x++) {
		for (unsigned int y = 0; y < chunkSize; y++) {
			for (unsigned int z = 0; z < chunkSize; z++) {

				unsigned int blockType;
				if (y <= 9) {
					blockType = Blocks::STONE_BLOCK;
				}
				else if (y <= 14) {
					blockType = Blocks::DIRT_BLOCK;
				}
				else {
					blockType = Blocks::GRASS_BLOCK;
				}

				chunkData.push_back(blockType);
				// Obtener las coordenadas de textura del bloque
				Block block = Blocks::blocks[blockType];

				//north face
				int northIndex = x * chunkSize * chunkSize + y * chunkSize + (z - 1);
				if (z <= 0 || chunkData[northIndex] == Blocks::AIR) {
					vertices.push_back(Vertex{ glm::vec3(x + 1.0f, y + 0.0f, z + 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(1.0f, 0.0f, 0.0f), block.sideMin }); //arriba izquierda
					vertices.push_back(Vertex{ glm::vec3(x + 0.0f, y + 0.0f, z + 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), block.sideMax }); //arriba derecha
					vertices.push_back(Vertex{ glm::vec3(x + 1.0f, y + 1.0f, z + 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 1.0f), block.sideMax }); //abajo izquierda
					vertices.push_back(Vertex{ glm::vec3(x + 0.0f, y + 1.0f, z + 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(1.0f, 1.0f, 0.0f), block.sideMin }); //abajo derecha

					indices.push_back(currentVertices + 0);
					indices.push_back(currentVertices + 3);
					indices.push_back(currentVertices + 1);
					indices.push_back(currentVertices + 0);
					indices.push_back(currentVertices + 2);
					indices.push_back(currentVertices + 3);
					currentVertices += 4;
				}

				//south face
				int southIndex = x * chunkSize * chunkSize + 0 * chunkSize + y;
				if (z >= chunkSize - 1 || chunkData[southIndex] == Blocks::AIR) {
					vertices.push_back(Vertex{ glm::vec3(x + 0.0f, y + 0.0f, z + 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 1.0f), block.sideMin });
					vertices.push_back(Vertex{ glm::vec3(x + 1.0f, y + 0.0f, z + 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 1.0f), block.sideMax });
					vertices.push_back(Vertex{ glm::vec3(x + 0.0f, y + 1.0f, z + 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), block.sideMax });
					vertices.push_back(Vertex{ glm::vec3(x + 1.0f, y + 1.0f, z + 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), block.sideMin });

					indices.push_back(currentVertices + 0);
					indices.push_back(currentVertices + 3);
					indices.push_back(currentVertices + 1);
					indices.push_back(currentVertices + 0);
					indices.push_back(currentVertices + 2);
					indices.push_back(currentVertices + 3);
					currentVertices += 4;
				}

				//west face
				int westIndex = x * chunkSize * chunkSize + y * chunkSize + (z - 1);
				if (x <= 0 || chunkData[westIndex] == Blocks::AIR) {
					vertices.push_back(Vertex{ glm::vec3(x + 0.0f, y + 0.0f, z + 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), block.sideMin });
					vertices.push_back(Vertex{ glm::vec3(x + 0.0f, y + 0.0f, z + 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 1.0f), block.sideMax });
					vertices.push_back(Vertex{ glm::vec3(x + 0.0f, y + 1.0f, z + 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), block.sideMax });
					vertices.push_back(Vertex{ glm::vec3(x + 0.0f, y + 1.0f, z + 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), block.sideMin });

					indices.push_back(currentVertices + 0);
					indices.push_back(currentVertices + 3);
					indices.push_back(currentVertices + 1);
					indices.push_back(currentVertices + 0);
					indices.push_back(currentVertices + 2);
					indices.push_back(currentVertices + 3);
					currentVertices += 4;
				}

				//east face
				int eastIndex = x * chunkSize * chunkSize + y * chunkSize + z;
				if (x >= chunkSize - 1 || chunkData[eastIndex] == Blocks::AIR) {
					vertices.push_back(Vertex{ glm::vec3(x + 1.0f, y + 0.0f, z + 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), block.sideMin });
					vertices.push_back(Vertex{ glm::vec3(x + 1.0f, y + 0.0f, z + 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 1.0f), block.sideMax });
					vertices.push_back(Vertex{ glm::vec3(x + 1.0f, y + 1.0f, z + 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), block.sideMax });
					vertices.push_back(Vertex{ glm::vec3(x + 1.0f, y + 1.0f, z + 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), block.sideMin });

					indices.push_back(currentVertices + 0);
					indices.push_back(currentVertices + 3);
					indices.push_back(currentVertices + 1);
					indices.push_back(currentVertices + 0);
					indices.push_back(currentVertices + 2);
					indices.push_back(currentVertices + 3);
					currentVertices += 4;
				}

				//bottom face
				int bottomIndex = x * chunkSize * chunkSize + y * chunkSize + z;
				if (y <= 0 || chunkData[bottomIndex] == Blocks::AIR) {
					vertices.push_back(Vertex{ glm::vec3(x + 0.0f, y + 0.0f, z + 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), block.bottomMin });
					vertices.push_back(Vertex{ glm::vec3(x + 1.0f, y + 0.0f, z + 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), block.bottomMax });
					vertices.push_back(Vertex{ glm::vec3(x + 0.0f, y + 0.0f, z + 1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), block.bottomMax });
					vertices.push_back(Vertex{ glm::vec3(x + 1.0f, y + 0.0f, z + 1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), block.bottomMin });

					indices.push_back(currentVertices + 0);
					indices.push_back(currentVertices + 3);
					indices.push_back(currentVertices + 1);
					indices.push_back(currentVertices + 0);
					indices.push_back(currentVertices + 2);
					indices.push_back(currentVertices + 3);
					currentVertices += 4;
				}


				//top face
				int topIndex = x * chunkSize * chunkSize + y * chunkSize + z;
				if (y >= chunkSize - 1 || chunkData[topIndex] == Blocks::AIR) {
					vertices.push_back(Vertex{ glm::vec3(x + 0.0f, y + 1.0f, z + 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), block.topMin });
					vertices.push_back(Vertex{ glm::vec3(x + 1.0f, y + 1.0f, z + 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), block.topMax });
					vertices.push_back(Vertex{ glm::vec3(x + 0.0f, y + 1.0f, z + 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), block.topMax });
					vertices.push_back(Vertex{ glm::vec3(x + 1.0f, y + 1.0f, z + 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), block.topMin });

					indices.push_back(currentVertices + 0);
					indices.push_back(currentVertices + 1);
					indices.push_back(currentVertices + 3);
					indices.push_back(currentVertices + 0);
					indices.push_back(currentVertices + 3);
					indices.push_back(currentVertices + 2);
					currentVertices += 4;
				}
			}
		}
	}

	numTriangles = indices.size();

	mesh = new Mesh(vertices, indices, textures);
}

void Chunk::Render(Shader& shader, Camera& camera) {
	if (mesh) {
		mesh->Draw(shader, camera);
	}
	else {
		std::cout << "Chunk not generated" << std::endl;
	}
}

int Chunk::GetCubeCount() const {
	//contar cubos en chunk segun las caras
	int count = 0;
	for (unsigned int i = 0; i < chunkData.size(); i++) {
		if (chunkData[i] != 0) {
			count++;
		}
	}
	return count;
}

int Chunk::GetChunkCount() {
	return chunkCount;
}