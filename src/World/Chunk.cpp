#include "Chunk.h"
#include "../Renderer/Shader.h"
#include "../Renderer/Camera.h"
#include <glad/glad.h>
#include <cstring>
#include <cmath>

Chunk::Chunk(int x, int y, int z, WorldGen* wg)
	: position(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)),
	meshBuilt(false), meshDirty(true), chunkX(x), chunkY(y), chunkZ(z), worldGen(wg),
	vao(nullptr), vbo(nullptr), ebo(nullptr) {
}

Chunk::~Chunk() {
	if (vao) delete vao;
	if (vbo) delete vbo;
	if (ebo) delete ebo;
}

void Chunk::setWorldGen(WorldGen* wg) {
	worldGen = wg;
}

void Chunk::generate() {
	for (int lx = 0; lx < SIZE; ++lx) {
		for (int lz = 0; lz < SIZE; ++lz) {
			int worldX = chunkX * SIZE + lx;
			int worldZ = chunkZ * SIZE + lz;
			float heightF = worldGen ? worldGen->getHeight(worldX, worldZ) : 8.0f;
			int height = static_cast<int>(std::floor(heightF));

			for (int ly = 0; ly < SIZE; ++ly) {
				int worldY = chunkY * SIZE + ly;
				if (worldY < height - 2) {
					blocks[lx][ly][lz] = Block(BlockType::STONE);
				}
				else if (worldY < height) {
					blocks[lx][ly][lz] = Block(BlockType::DIRT);
				}
				else if (worldY == height) {
					blocks[lx][ly][lz] = Block(BlockType::GRASS);
				}
				else {
					blocks[lx][ly][lz] = Block(BlockType::AIR);
				}
			}
		}
	}
}

// Helper to get block type safely
static BlockType getBlockSafe(const Block blocks[Chunk::SIZE][Chunk::SIZE][Chunk::SIZE], int x, int y, int z) {
	if (x < 0 || x >= Chunk::SIZE || y < 0 || y >= Chunk::SIZE || z < 0 || z >= Chunk::SIZE) {
		return BlockType::AIR;
	}
	return blocks[x][y][z].type;
}

// Check neighbor with chunk boundary support
static BlockType getNeighborBlock(const Block blocks[Chunk::SIZE][Chunk::SIZE][Chunk::SIZE],
	const std::map<std::tuple<int, int, int>, Chunk*>& allChunks,
	int chunkX, int chunkY, int chunkZ,
	int x, int y, int z) {
	// Check if within current chunk
	if (x >= 0 && x < Chunk::SIZE && y >= 0 && y < Chunk::SIZE && z >= 0 && z < Chunk::SIZE) {
		return blocks[x][y][z].type;
	}

	// Calculate neighbor chunk coordinates
	int neighborChunkX = chunkX;
	int neighborChunkY = chunkY;
	int neighborChunkZ = chunkZ;
	int localX = x;
	int localY = y;
	int localZ = z;

	if (x < 0) { neighborChunkX--; localX = Chunk::SIZE - 1; }
	else if (x >= Chunk::SIZE) { neighborChunkX++; localX = 0; }

	if (y < 0) { neighborChunkY--; localY = Chunk::SIZE - 1; }
	else if (y >= Chunk::SIZE) { neighborChunkY++; localY = 0; }

	if (z < 0) { neighborChunkZ--; localZ = Chunk::SIZE - 1; }
	else if (z >= Chunk::SIZE) { neighborChunkZ++; localZ = 0; }

	auto key = std::make_tuple(neighborChunkX, neighborChunkY, neighborChunkZ);
	auto it = allChunks.find(key);
	if (it != allChunks.end()) {
		return it->second->blocks[localX][localY][localZ].type;
	}

	return BlockType::AIR;
}

void Chunk::rebuildMesh(const std::map<std::tuple<int, int, int>, Chunk*>& allChunks) {
	std::vector<float> verts;
	verts.reserve(Chunk::SIZE * Chunk::SIZE * Chunk::SIZE * 24);
	indices.clear();
	indices.reserve(Chunk::SIZE * Chunk::SIZE * Chunk::SIZE * 36);

	// Atlas layout: 2x2
	// (0,0) = stone, (1,0) = grass top, (0,1) = dirt, (1,1) = grass side
	const float tileSize = 0.5f;

	auto getUV = [](int col, int row, float u, float v) {
		float uu = col * 0.5f + u * 0.5f;
		float vv = 1.0f - ((row + 1) * 0.5f) + v * 0.5f;
		return std::make_pair(uu, vv);
		};

	auto getTile = [](BlockType type, int face) {
		// face: 0=+X, 1=-X, 2=+Y(top), 3=-Y(bottom), 4=+Z, 5=-Z
		if (type == BlockType::GRASS) {
			if (face == 2) return std::make_pair(1, 0); // top
			if (face == 3) return std::make_pair(0, 1); // bottom = dirt
			return std::make_pair(1, 1); // sides
		}
		else if (type == BlockType::DIRT) {
			return std::make_pair(0, 1);
		}
		else { // STONE
			return std::make_pair(0, 0);
		}
		};

	// Generate mesh - simple face culling (no greedy meshing)
	for (int x = 0; x < SIZE; ++x) {
		for (int y = 0; y < SIZE; ++y) {
			for (int z = 0; z < SIZE; ++z) {
				BlockType blockType = blocks[x][y][z].type;
				if (blockType == BlockType::AIR) continue;

				// World position
				float worldX = static_cast<float>(chunkX * SIZE + x);
				float worldY = static_cast<float>(chunkY * SIZE + y);
				float worldZ = static_cast<float>(chunkZ * SIZE + z);

				float x0 = worldX - 0.5f;
				float y0 = worldY - 0.5f;
				float z0 = worldZ - 0.5f;
				float x1 = x0 + 1.0f;
				float y1 = y0 + 1.0f;
				float z1 = z0 + 1.0f;

				// Check each face
				// +X face (right)
				if (getNeighborBlock(blocks, allChunks, chunkX, chunkY, chunkZ, x + 1, y, z) == BlockType::AIR) {
					auto tile = getTile(blockType, 0);
					auto uv0 = getUV(tile.first, tile.second, 0, 0);
					auto uv1 = getUV(tile.first, tile.second, 1, 0);
					auto uv2 = getUV(tile.first, tile.second, 1, 1);
					auto uv3 = getUV(tile.first, tile.second, 0, 1);

					unsigned int base = static_cast<unsigned int>(verts.size() / 5);
					verts.insert(verts.end(), { x1, y0, z1, uv0.first, uv0.second });
					verts.insert(verts.end(), { x1, y0, z0, uv1.first, uv1.second });
					verts.insert(verts.end(), { x1, y1, z0, uv2.first, uv2.second });
					verts.insert(verts.end(), { x1, y1, z1, uv3.first, uv3.second });
					indices.insert(indices.end(), { base, base + 1, base + 2, base + 2, base + 3, base });
				}

				// -X face (left)
				if (getNeighborBlock(blocks, allChunks, chunkX, chunkY, chunkZ, x - 1, y, z) == BlockType::AIR) {
					auto tile = getTile(blockType, 1);
					auto uv0 = getUV(tile.first, tile.second, 0, 0);
					auto uv1 = getUV(tile.first, tile.second, 1, 0);
					auto uv2 = getUV(tile.first, tile.second, 1, 1);
					auto uv3 = getUV(tile.first, tile.second, 0, 1);

					unsigned int base = static_cast<unsigned int>(verts.size() / 5);
					verts.insert(verts.end(), { x0, y0, z0, uv0.first, uv0.second });
					verts.insert(verts.end(), { x0, y0, z1, uv1.first, uv1.second });
					verts.insert(verts.end(), { x0, y1, z1, uv2.first, uv2.second });
					verts.insert(verts.end(), { x0, y1, z0, uv3.first, uv3.second });
					indices.insert(indices.end(), { base, base + 1, base + 2, base + 2, base + 3, base });
				}

				// +Y face (top)
				if (getNeighborBlock(blocks, allChunks, chunkX, chunkY, chunkZ, x, y + 1, z) == BlockType::AIR) {
					auto tile = getTile(blockType, 2);
					auto uv0 = getUV(tile.first, tile.second, 0, 0);
					auto uv1 = getUV(tile.first, tile.second, 1, 0);
					auto uv2 = getUV(tile.first, tile.second, 1, 1);
					auto uv3 = getUV(tile.first, tile.second, 0, 1);

					unsigned int base = static_cast<unsigned int>(verts.size() / 5);
					verts.insert(verts.end(), { x0, y1, z1, uv0.first, uv0.second });
					verts.insert(verts.end(), { x1, y1, z1, uv1.first, uv1.second });
					verts.insert(verts.end(), { x1, y1, z0, uv2.first, uv2.second });
					verts.insert(verts.end(), { x0, y1, z0, uv3.first, uv3.second });
					indices.insert(indices.end(), { base, base + 1, base + 2, base + 2, base + 3, base });
				}

				// -Y face (bottom)
				if (getNeighborBlock(blocks, allChunks, chunkX, chunkY, chunkZ, x, y - 1, z) == BlockType::AIR) {
					auto tile = getTile(blockType, 3);
					auto uv0 = getUV(tile.first, tile.second, 0, 0);
					auto uv1 = getUV(tile.first, tile.second, 1, 0);
					auto uv2 = getUV(tile.first, tile.second, 1, 1);
					auto uv3 = getUV(tile.first, tile.second, 0, 1);

					unsigned int base = static_cast<unsigned int>(verts.size() / 5);
					verts.insert(verts.end(), { x0, y0, z0, uv0.first, uv0.second });
					verts.insert(verts.end(), { x1, y0, z0, uv1.first, uv1.second });
					verts.insert(verts.end(), { x1, y0, z1, uv2.first, uv2.second });
					verts.insert(verts.end(), { x0, y0, z1, uv3.first, uv3.second });
					indices.insert(indices.end(), { base, base + 1, base + 2, base + 2, base + 3, base });
				}

				// +Z face (front)
				if (getNeighborBlock(blocks, allChunks, chunkX, chunkY, chunkZ, x, y, z + 1) == BlockType::AIR) {
					auto tile = getTile(blockType, 4);
					auto uv0 = getUV(tile.first, tile.second, 0, 0);
					auto uv1 = getUV(tile.first, tile.second, 1, 0);
					auto uv2 = getUV(tile.first, tile.second, 1, 1);
					auto uv3 = getUV(tile.first, tile.second, 0, 1);

					unsigned int base = static_cast<unsigned int>(verts.size() / 5);
					verts.insert(verts.end(), { x0, y0, z1, uv0.first, uv0.second });
					verts.insert(verts.end(), { x1, y0, z1, uv1.first, uv1.second });
					verts.insert(verts.end(), { x1, y1, z1, uv2.first, uv2.second });
					verts.insert(verts.end(), { x0, y1, z1, uv3.first, uv3.second });
					indices.insert(indices.end(), { base, base + 1, base + 2, base + 2, base + 3, base });
				}

				// -Z face (back)
				if (getNeighborBlock(blocks, allChunks, chunkX, chunkY, chunkZ, x, y, z - 1) == BlockType::AIR) {
					auto tile = getTile(blockType, 5);
					auto uv0 = getUV(tile.first, tile.second, 0, 0);
					auto uv1 = getUV(tile.first, tile.second, 1, 0);
					auto uv2 = getUV(tile.first, tile.second, 1, 1);
					auto uv3 = getUV(tile.first, tile.second, 0, 1);

					unsigned int base = static_cast<unsigned int>(verts.size() / 5);
					verts.insert(verts.end(), { x1, y0, z0, uv0.first, uv0.second });
					verts.insert(verts.end(), { x0, y0, z0, uv1.first, uv1.second });
					verts.insert(verts.end(), { x0, y1, z0, uv2.first, uv2.second });
					verts.insert(verts.end(), { x1, y1, z0, uv3.first, uv3.second });
					indices.insert(indices.end(), { base, base + 1, base + 2, base + 2, base + 3, base });
				}
			}
		}
	}

	// Store raw vertex data
	if (!verts.empty()) {
		rawVertices.resize(verts.size() * sizeof(float));
		std::memcpy(rawVertices.data(), verts.data(), rawVertices.size());
		meshBuilt = true;
	}
	else {
		meshBuilt = false;
		rawVertices.clear();
		indices.clear();
	}
}

void Chunk::draw(Shader& shader, Camera& camera) {
	if (!meshBuilt || !vao) return;

	shader.use();
	glm::mat4 model = glm::mat4(1.0f); // Identity - vertices already in world space
	shader.setMat4("model", model);
	shader.setMat4("view", camera.GetViewMatrix());
	shader.setMat4("projection", camera.GetProjectionMatrix());

	vao->bind();
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
	vao->unbind();
}