#include "Header_files/chunk.h"

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

				//chunkData.push_back(0); //empty block
				chunkData.push_back(1); //solid block
				//chunkData.push_back(((x + y + z) % 2 == 0) ? 1 : 0); //checkerboard patern

				//north face
				int northIndex = x * chunkSize * chunkSize + y * chunkSize + (z - 1);
				if (z <= 0 || chunkData[northIndex] == 0) {
					vertices.push_back(Vertex{ glm::vec3(x + 1.0f, y + 0.0f, z + 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(16.0f / 32.0f, 0.0f / 32.0f)});
					vertices.push_back(Vertex{ glm::vec3(x + 0.0f, y + 0.0f, z + 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(32.0f / 32.0f, 0.0f / 32.0f)});
					vertices.push_back(Vertex{ glm::vec3(x + 1.0f, y + 1.0f, z + 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(32.0f / 32.0f, 16.0f / 32.0f)});
					vertices.push_back(Vertex{ glm::vec3(x + 0.0f, y + 1.0f, z + 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(16.0f / 32.0f, 16.0f / 32.0f)});

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
				if (z >= chunkSize - 1 || chunkData[southIndex] == 0) {
					vertices.push_back(Vertex{ glm::vec3(x + 0.0f, y + 0.0f, z + 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec2(16.0f / 32.0f, 0.0f / 32.0f)});
					vertices.push_back(Vertex{ glm::vec3(x + 1.0f, y + 0.0f, z + 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 1.0f), glm::vec2(32.0f / 32.0f, 0.0f / 32.0f)});
					vertices.push_back(Vertex{ glm::vec3(x + 0.0f, y + 1.0f, z + 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(32.0f / 32.0f, 16.0f / 32.0f)});
					vertices.push_back(Vertex{ glm::vec3(x + 1.0f, y + 1.0f, z + 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(16.0f / 32.0f, 16.0f / 32.0f)});

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
				if (x <= 0 || chunkData[westIndex] == 0) {
					vertices.push_back(Vertex{ glm::vec3(x + 0.0f, y + 0.0f, z + 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(16.0f / 32.0f, 0.0f / 32.0f)});
					vertices.push_back(Vertex{ glm::vec3(x + 0.0f, y + 0.0f, z + 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec2(32.0f / 32.0f, 0.0f / 32.0f)});
					vertices.push_back(Vertex{ glm::vec3(x + 0.0f, y + 1.0f, z + 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(32.0f / 32.0f, 16.0f / 32.0f)});
					vertices.push_back(Vertex{ glm::vec3(x + 0.0f, y + 1.0f, z + 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(16.0f / 32.0f, 16.0f / 32.0f)});

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
				if (x >= chunkSize - 1 || chunkData[eastIndex] == 0) {
					vertices.push_back(Vertex{ glm::vec3(x + 1.0f, y + 0.0f, z + 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(16.0f / 32.0f, 0.0f / 32.0f)});
					vertices.push_back(Vertex{ glm::vec3(x + 1.0f, y + 0.0f, z + 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 1.0f), glm::vec2(32.0f / 32.0f, 0.0f / 32.0f)});
					vertices.push_back(Vertex{ glm::vec3(x + 1.0f, y + 1.0f, z + 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(32.0f / 32.0f, 16.0f / 32.0f)});
					vertices.push_back(Vertex{ glm::vec3(x + 1.0f, y + 1.0f, z + 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(16.0f / 32.0f, 16.0f / 32.0f)});

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
				if (y <= 0 || chunkData[bottomIndex] == 0) {
					vertices.push_back(Vertex{ glm::vec3(x + 0.0f, y + 0.0f, z + 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f / 32.0f, 1.0f - 32.0f / 32.0f)});
					vertices.push_back(Vertex{ glm::vec3(x + 1.0f, y + 0.0f, z + 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(16.0f / 32.0f, 1.0f - 32.0f / 32.0f)});
					vertices.push_back(Vertex{ glm::vec3(x + 0.0f, y + 0.0f, z + 1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(16.0f / 32.0f, 1.0f - 32.0f / 32.0f)});
					vertices.push_back(Vertex{ glm::vec3(x + 1.0f, y + 0.0f, z + 1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(0.0f / 32.0f, 1.0f - 32.0f / 32.0f)});

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
				if (y >= chunkSize - 1 || chunkData[topIndex] == 0) {
					vertices.push_back(Vertex{ glm::vec3(x + 0.0f, y + 1.0f, z + 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(16.0f / 32.0f, 1.0f - 0.0f / 32.0f)});
					vertices.push_back(Vertex{ glm::vec3(x + 1.0f, y + 1.0f, z + 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(32.0f / 32.0f, 1.0f - 0.0f / 32.0f)});
					vertices.push_back(Vertex{ glm::vec3(x + 0.0f, y + 1.0f, z + 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(32.0f / 32.0f, 1.0f - 16.0f / 32.0f)});
					vertices.push_back(Vertex{ glm::vec3(x + 1.0f, y + 1.0f, z + 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(16.0f / 32.0f, 1.0f - 16.0f / 32.0f)});

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
	return chunkSize * chunkSize * chunkSize;
}

int Chunk::GetChunkCount() {
	return chunkCount;
}