#include "Header_files/chunk.h"
#include "Header_files/vao.h"
#include "Header_files/vbo.h"
#include "Header_files/ebo.h"
#include "Header_files/Camera.h"

int Chunk::chunkCount = 0;

Chunk::Chunk(unsigned int chunkSize, glm::vec3 chunkPos)
	: chunkSize(chunkSize), numTriangles(0) {
	chunkCount++;
	GenerateChunk();
}

Chunk::~Chunk() {
	chunkCount--;
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
}

void Chunk::GenerateChunk() {
	std::vector<ChunkVertex> vertices;
	std::vector<unsigned int> indices;

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
					vertices.push_back(ChunkVertex(glm::vec3(x + 1.0f, y + 0.0f, z + 0.0f), glm::vec2(16.0f / 32.0f, 0.0f / 32.0f)));
					vertices.push_back(ChunkVertex(glm::vec3(x + 0.0f, y + 0.0f, z + 0.0f), glm::vec2(32.0f / 32.0f, 0.0f / 32.0f)));
					vertices.push_back(ChunkVertex(glm::vec3(x + 1.0f, y + 1.0f, z + 0.0f), glm::vec2(32.0f / 32.0f, 16.0f / 32.0f)));
					vertices.push_back(ChunkVertex(glm::vec3(x + 0.0f, y + 1.0f, z + 0.0f), glm::vec2(16.0f / 32.0f, 16.0f / 32.0f)));

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
					vertices.push_back(ChunkVertex(glm::vec3(x + 0.0f, y + 0.0f, z + 1.0f), glm::vec2(16.0f / 32.0f, 0.0f / 32.0f)));
					vertices.push_back(ChunkVertex(glm::vec3(x + 1.0f, y + 0.0f, z + 1.0f), glm::vec2(32.0f / 32.0f, 0.0f / 32.0f)));
					vertices.push_back(ChunkVertex(glm::vec3(x + 0.0f, y + 1.0f, z + 1.0f), glm::vec2(32.0f / 32.0f, 16.0f / 32.0f)));
					vertices.push_back(ChunkVertex(glm::vec3(x + 1.0f, y + 1.0f, z + 1.0f), glm::vec2(16.0f / 32.0f, 16.0f / 32.0f)));

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
					vertices.push_back(ChunkVertex(glm::vec3(x + 0.0f, y + 0.0f, z + 0.0f), glm::vec2(16.0f / 32.0f, 0.0f / 32.0f)));
					vertices.push_back(ChunkVertex(glm::vec3(x + 0.0f, y + 0.0f, z + 1.0f), glm::vec2(32.0f / 32.0f, 0.0f / 32.0f)));
					vertices.push_back(ChunkVertex(glm::vec3(x + 0.0f, y + 1.0f, z + 0.0f), glm::vec2(32.0f / 32.0f, 16.0f / 32.0f)));
					vertices.push_back(ChunkVertex(glm::vec3(x + 0.0f, y + 1.0f, z + 1.0f), glm::vec2(16.0f / 32.0f, 16.0f / 32.0f)));

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
					vertices.push_back(ChunkVertex(glm::vec3(x + 1.0f, y + 0.0f, z + 1.0f), glm::vec2(16.0f / 32.0f, 0.0f / 32.0f)));
					vertices.push_back(ChunkVertex(glm::vec3(x + 1.0f, y + 0.0f, z + 0.0f), glm::vec2(32.0f / 32.0f, 0.0f / 32.0f)));
					vertices.push_back(ChunkVertex(glm::vec3(x + 1.0f, y + 1.0f, z + 1.0f), glm::vec2(32.0f / 32.0f, 16.0f / 32.0f)));
					vertices.push_back(ChunkVertex(glm::vec3(x + 1.0f, y + 1.0f, z + 0.0f), glm::vec2(16.0f / 32.0f, 16.0f / 32.0f)));

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
					vertices.push_back(ChunkVertex(glm::vec3(x + 0.0f, y + 0.0f, z + 0.0f), glm::vec2(0.0f / 32.0f, 1.0f - 32.0f / 32.0f)));
					vertices.push_back(ChunkVertex(glm::vec3(x + 1.0f, y + 0.0f, z + 0.0f), glm::vec2(16.0f / 32.0f, 1.0f - 32.0f / 32.0f)));
					vertices.push_back(ChunkVertex(glm::vec3(x + 0.0f, y + 0.0f, z + 1.0f), glm::vec2(16.0f / 32.0f, 1.0f - 32.0f / 32.0f)));
					vertices.push_back(ChunkVertex(glm::vec3(x + 1.0f, y + 0.0f, z + 1.0f), glm::vec2(0.0f / 32.0f, 1.0f - 32.0f / 32.0f)));

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
					vertices.push_back(ChunkVertex(glm::vec3(x + 0.0f, y + 1.0f, z + 0.0f), glm::vec2(16.0f / 32.0f, 1.0f - 0.0f / 32.0f)));
					vertices.push_back(ChunkVertex(glm::vec3(x + 1.0f, y + 1.0f, z + 0.0f), glm::vec2(32.0f / 32.0f, 1.0f - 0.0f / 32.0f)));
					vertices.push_back(ChunkVertex(glm::vec3(x + 0.0f, y + 1.0f, z + 1.0f), glm::vec2(32.0f / 32.0f, 1.0f - 16.0f / 32.0f)));
					vertices.push_back(ChunkVertex(glm::vec3(x + 1.0f, y + 1.0f, z + 1.0f), glm::vec2(16.0f / 32.0f, 1.0f - 16.0f / 32.0f)));

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

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(ChunkVertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// Atributos de vértices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ChunkVertex), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(ChunkVertex), (void*)offsetof(ChunkVertex, texCoords));
	glEnableVertexAttribArray(3);

	glBindVertexArray(0);
}

void Chunk::Render(Shader& shader, Camera& camera) {
	shader.use();
	glBindVertexArray(vao);

	unsigned int numDiffuse = 0;
	unsigned int numSpecular = 0;

	glUniform3f(glGetUniformLocation(shader.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
	camera.Matrix(shader, "camMatrix");

	glDrawElements(GL_TRIANGLES, numTriangles, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

int Chunk::GetCubeCount() const {
	return chunkSize * chunkSize * chunkSize;
}

int Chunk::GetChunkCount() {
	return chunkCount;
}