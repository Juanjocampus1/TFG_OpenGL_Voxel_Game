#include "Header_files/chunk.h"

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

	unsigned int currentVertices = 0;
	for (int x = 0; x < chunkSize; x++) {
		for (int y = 0; y < chunkSize; y++) {
			for (int z = 0; z < chunkSize; z++) {

				//north face
				vertices.push_back(ChunkVertex(glm::vec3(x + 1.0f, y + 0.0f, z + 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(16.0f / 32.0f, 0.0f / 32.0f)));
				vertices.push_back(ChunkVertex(glm::vec3(x + 0.0f, y + 0.0f, z + 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(32.0f / 32.0f, 0.0f / 32.0f)));
				vertices.push_back(ChunkVertex(glm::vec3(x + 1.0f, y + 1.0f, z + 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(32.0f / 32.0f, 16.0f / 32.0f)));
				vertices.push_back(ChunkVertex(glm::vec3(x + 0.0f, y + 1.0f, z + 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(16.0f / 32.0f, 16.0f / 32.0f)));
			
				indices.push_back(currentVertices + 0);
				indices.push_back(currentVertices + 3);
				indices.push_back(currentVertices + 1);
				indices.push_back(currentVertices + 0);
				indices.push_back(currentVertices + 2);
				indices.push_back(currentVertices + 3);
				currentVertices += 4;

				//south face
				vertices.push_back(ChunkVertex(glm::vec3(x + 0.0f, y + 0.0f, z + 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec2(16.0f / 32.0f, 0.0f / 32.0f)));
				vertices.push_back(ChunkVertex(glm::vec3(x + 1.0f, y + 0.0f, z + 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 1.0f), glm::vec2(32.0f / 32.0f, 0.0f / 32.0f)));
				vertices.push_back(ChunkVertex(glm::vec3(x + 0.0f, y + 1.0f, z + 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(32.0f / 32.0f, 16.0f / 32.0f)));
				vertices.push_back(ChunkVertex(glm::vec3(x + 1.0f, y + 1.0f, z + 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(16.0f / 32.0f, 16.0f / 32.0f)));

				indices.push_back(currentVertices + 0);
				indices.push_back(currentVertices + 3);
				indices.push_back(currentVertices + 1);
				indices.push_back(currentVertices + 0);
				indices.push_back(currentVertices + 2);
				indices.push_back(currentVertices + 3);
				currentVertices += 4;

				//west face
				vertices.push_back(ChunkVertex(glm::vec3(x + 0.0f, y + 0.0f, z + 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(16.0f / 32.0f, 0.0f / 32.0f)));
				vertices.push_back(ChunkVertex(glm::vec3(x + 0.0f, y + 0.0f, z + 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec2(32.0f / 32.0f, 0.0f / 32.0f)));
				vertices.push_back(ChunkVertex(glm::vec3(x + 0.0f, y + 1.0f, z + 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(32.0f / 32.0f, 16.0f / 32.0f)));
				vertices.push_back(ChunkVertex(glm::vec3(x + 0.0f, y + 1.0f, z + 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(16.0f / 32.0f, 16.0f / 32.0f)));

				indices.push_back(currentVertices + 0);
				indices.push_back(currentVertices + 3);
				indices.push_back(currentVertices + 1);
				indices.push_back(currentVertices + 0);
				indices.push_back(currentVertices + 2);
				indices.push_back(currentVertices + 3);
				currentVertices += 4;

				//east face
				vertices.push_back(ChunkVertex(glm::vec3(x + 1.0f, y + 0.0f, z + 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(16.0f / 32.0f, 0.0f / 32.0f)));
				vertices.push_back(ChunkVertex(glm::vec3(x + 1.0f, y + 0.0f, z + 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 1.0f), glm::vec2(32.0f / 32.0f, 0.0f / 32.0f)));
				vertices.push_back(ChunkVertex(glm::vec3(x + 1.0f, y + 1.0f, z + 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(32.0f / 32.0f, 16.0f / 32.0f)));
				vertices.push_back(ChunkVertex(glm::vec3(x + 1.0f, y + 1.0f, z + 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(16.0f / 32.0f, 16.0f / 32.0f)));

				indices.push_back(currentVertices + 0);
				indices.push_back(currentVertices + 3);
				indices.push_back(currentVertices + 1);
				indices.push_back(currentVertices + 0);
				indices.push_back(currentVertices + 2);
				indices.push_back(currentVertices + 3);
				currentVertices += 4;

				//bottom face
				vertices.push_back(ChunkVertex(glm::vec3(x + 0.0f, y + 0.0f, z + 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f / 32.0f, 1.0f - 32.0f / 32.0f)));
				vertices.push_back(ChunkVertex(glm::vec3(x + 1.0f, y + 0.0f, z + 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(16.0f / 32.0f, 1.0f - 32.0f / 32.0f)));
				vertices.push_back(ChunkVertex(glm::vec3(x + 0.0f, y + 0.0f, z + 1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(16.0f / 32.0f, 1.0f - 32.0f / 32.0f)));
				vertices.push_back(ChunkVertex(glm::vec3(x + 1.0f, y + 0.0f, z + 1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(0.0f / 32.0f, 1.0f - 32.0f / 32.0f)));

				indices.push_back(currentVertices + 0);
				indices.push_back(currentVertices + 3);
				indices.push_back(currentVertices + 1);
				indices.push_back(currentVertices + 0);
				indices.push_back(currentVertices + 2);
				indices.push_back(currentVertices + 3);
				currentVertices += 4;

				//top face
				vertices.push_back(ChunkVertex(glm::vec3(x + 0.0f, y + 1.0f, z + 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(16.0f / 32.0f, 1.0f - 0.0f / 32.0f)));
				vertices.push_back(ChunkVertex(glm::vec3(x + 1.0f, y + 1.0f, z + 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(32.0f / 32.0f, 1.0f - 0.0f / 32.0f)));
				vertices.push_back(ChunkVertex(glm::vec3(x + 0.0f, y + 1.0f, z + 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(32.0f / 32.0f, 1.0f - 16.0f / 32.0f)));
				vertices.push_back(ChunkVertex(glm::vec3(x + 1.0f, y + 1.0f, z + 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(16.0f / 32.0f, 1.0f - 16.0f / 32.0f)));

				indices.push_back(currentVertices + 0);
				indices.push_back(currentVertices + 3);
				indices.push_back(currentVertices + 1);
				indices.push_back(currentVertices + 0);
				indices.push_back(currentVertices + 2);
				indices.push_back(currentVertices + 3);
				currentVertices += 4;
			}
		}
	}

	numTriangles = indices.size() / 3;

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(ChunkVertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_BYTE, GL_FALSE, sizeof(ChunkVertex), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_BYTE, GL_FALSE, sizeof(ChunkVertex), (void*)(3 * sizeof(char)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Chunk::Render(Shader& shader) {
	shader.use();
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, numTriangles * 3, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

int Chunk::GetCubeCount() const {
	return chunkSize * chunkSize * chunkSize;
}

int Chunk::GetChunkCount() {
	return chunkCount;
}