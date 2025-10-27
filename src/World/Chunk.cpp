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
	generate();
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
	for (int lx =0; lx < SIZE; ++lx) {
		for (int lz =0; lz < SIZE; ++lz) {
			int worldX = chunkX * SIZE + lx;
			int worldZ = chunkZ * SIZE + lz;
			float heightF = worldGen ? worldGen->getHeight(worldX, worldZ) :8.0f;
			int height = static_cast<int>(std::floor(heightF));
			for (int ly =0; ly < SIZE; ++ly) {
				int worldY = chunkY * SIZE + ly;
				if (worldY < height -2) {
					blocks[lx][ly][lz] = Block(BlockType::STONE);
				} else if (worldY < height) {
					blocks[lx][ly][lz] = Block(BlockType::DIRT);
				} else if (worldY == height) {
					blocks[lx][ly][lz] = Block(BlockType::GRASS);
				} else {
					blocks[lx][ly][lz] = Block(BlockType::AIR);
				}
			}
		}
	}
}

void Chunk::rebuildMesh(const std::map<std::tuple<int, int, int>, Chunk*>& allChunks) {
	std::vector<float> verts;
	indices.clear();
	unsigned int vertexCount =0;

	// Atlas params (2x2)
	const int atlasCols =2;
	const int atlasRows =2;
	const float tileU =1.0f / atlasCols;
	const float tileV =1.0f / atlasRows;

	auto tileUV = [&](int col, int row, float uIndex, float vIndex) {
		float u = col * tileU + uIndex * tileU;
		// Flip V so row0 (top row in atlas) maps correctly: v =1 - (row+1)*tileV + vIndex*tileV
		float v =1.0f - (row +1) * tileV + vIndex * tileV;
		return std::make_pair(u, v);
	};

	for (int x =0; x < SIZE; ++x) {
		for (int y =0; y < SIZE; ++y) {
			for (int z =0; z < SIZE; ++z) {
				if (blocks[x][y][z].type == BlockType::AIR) continue;

				BlockType type = blocks[x][y][z].type;
				// determine tile indices: col, row (0-based, col left->right, row top->bottom)
				int sideCol =1, sideRow =1; // bottom-right by default (grass side)
				int topCol =1, topRow =0; // top-right (grass top)
				int dirtCol =0, dirtRow =1; // bottom-left (dirt)
				int stoneCol =0, stoneRow =0; // top-left (stone)

				int useSideCol = stoneCol, useSideRow = stoneRow;
				int useTopCol = stoneCol, useTopRow = stoneRow;
				int useBottomCol = stoneCol, useBottomRow = stoneRow;

				if (type == BlockType::GRASS) {
					useTopCol = topCol; useTopRow = topRow;
					useSideCol = sideCol; useSideRow = sideRow;
					useBottomCol = dirtCol; useBottomRow = dirtRow;
				} else if (type == BlockType::DIRT) {
					useTopCol = dirtCol; useTopRow = dirtRow;
					useSideCol = dirtCol; useSideRow = dirtRow;
					useBottomCol = dirtCol; useBottomRow = dirtRow;
				} else if (type == BlockType::STONE) {
					useTopCol = stoneCol; useTopRow = stoneRow;
					useSideCol = stoneCol; useSideRow = stoneRow;
					useBottomCol = stoneCol; useBottomRow = stoneRow;
				}

				// Front face (z+)
				{
					bool shouldRender = true;
					if (z +1 < SIZE) {
						if (blocks[x][y][z +1].type != BlockType::AIR) shouldRender = false;
					} else {
						auto it = allChunks.find(std::make_tuple(chunkX, chunkY, chunkZ +1));
						if (it != allChunks.end() && it->second->blocks[x][y][0].type != BlockType::AIR) {
							shouldRender = false;
						}
					}
					if (shouldRender) {
						auto p00 = tileUV(useSideCol, useSideRow,0.0f,0.0f);
						auto p10 = tileUV(useSideCol, useSideRow,1.0f,0.0f);
						auto p11 = tileUV(useSideCol, useSideRow,1.0f,1.0f);
						auto p01 = tileUV(useSideCol, useSideRow,0.0f,1.0f);
						verts.insert(verts.end(), {
							(float)x -0.5f, (float)y -0.5f, (float)z +0.5f, p00.first, p00.second,
							(float)x +0.5f, (float)y -0.5f, (float)z +0.5f, p10.first, p10.second,
							(float)x +0.5f, (float)y +0.5f, (float)z +0.5f, p11.first, p11.second,
							(float)x -0.5f, (float)y +0.5f, (float)z +0.5f, p01.first, p01.second
						});
						indices.insert(indices.end(), { vertexCount, vertexCount +1, vertexCount +2, vertexCount +2, vertexCount +3, vertexCount });
						vertexCount +=4;
					}
				}

				// Back face (z-)
				{
					bool shouldRender = true;
					if (z -1 >=0) {
						if (blocks[x][y][z -1].type != BlockType::AIR) shouldRender = false;
					} else {
						auto it = allChunks.find(std::make_tuple(chunkX, chunkY, chunkZ -1));
						if (it == allChunks.end()) { /*missingNeighbor = true; break;*/ }
						if (it != allChunks.end() && it->second->blocks[x][y][SIZE -1].type != BlockType::AIR) shouldRender = false;
					}
					if (shouldRender) {
						auto p00 = tileUV(useSideCol, useSideRow,0.0f,0.0f);
						auto p01 = tileUV(useSideCol, useSideRow,0.0f,1.0f);
						auto p11 = tileUV(useSideCol, useSideRow,1.0f,1.0f);
						auto p10 = tileUV(useSideCol, useSideRow,1.0f,0.0f);
						verts.insert(verts.end(), {
							(float)x -0.5f, (float)y -0.5f, (float)z -0.5f, p00.first, p00.second,
							(float)x -0.5f, (float)y +0.5f, (float)z -0.5f, p01.first, p01.second,
							(float)x +0.5f, (float)y +0.5f, (float)z -0.5f, p11.first, p11.second,
							(float)x +0.5f, (float)y -0.5f, (float)z -0.5f, p10.first, p10.second
						});
						indices.insert(indices.end(), { vertexCount, vertexCount +1, vertexCount +2, vertexCount +2, vertexCount +3, vertexCount });
						vertexCount +=4;
					}
				}

				// Left face (x-)
				{
					bool shouldRender = true;
					if (x -1 >=0) {
						if (blocks[x -1][y][z].type != BlockType::AIR) shouldRender = false;
					} else {
						auto it = allChunks.find(std::make_tuple(chunkX -1, chunkY, chunkZ));
						if (it == allChunks.end()) { /*missingNeighbor = true; break;*/ }
						if (it != allChunks.end() && it->second->blocks[SIZE -1][y][z].type != BlockType::AIR) shouldRender = false;
					}
					if (shouldRender) {
						auto p00 = tileUV(useSideCol, useSideRow,0.0f,1.0f);
						auto p10 = tileUV(useSideCol, useSideRow,1.0f,1.0f);
						auto p11 = tileUV(useSideCol, useSideRow,1.0f,0.0f);
						auto p01 = tileUV(useSideCol, useSideRow,0.0f,0.0f);
						verts.insert(verts.end(), {
							(float)x -0.5f, (float)y +0.5f, (float)z +0.5f, p00.first, p00.second,
							(float)x -0.5f, (float)y +0.5f, (float)z -0.5f, p10.first, p10.second,
							(float)x -0.5f, (float)y -0.5f, (float)z -0.5f, p11.first, p11.second,
							(float)x -0.5f, (float)y -0.5f, (float)z +0.5f, p01.first, p01.second
						});
						indices.insert(indices.end(), { vertexCount, vertexCount +1, vertexCount +2, vertexCount +2, vertexCount +3, vertexCount });
						vertexCount +=4;
					}
				}

				// Right face (x+)
				{
					bool shouldRender = true;
					if (x +1 < SIZE) {
						if (blocks[x +1][y][z].type != BlockType::AIR) shouldRender = false;
					} else {
						auto it = allChunks.find(std::make_tuple(chunkX +1, chunkY, chunkZ));
						if (it == allChunks.end()) { /*missingNeighbor = true; break;*/ }
						if (it != allChunks.end() && it->second->blocks[0][y][z].type != BlockType::AIR) shouldRender = false;
					}
					if (shouldRender) {
						auto p00 = tileUV(useSideCol, useSideRow,0.0f,1.0f);
						auto p01 = tileUV(useSideCol, useSideRow,0.0f,0.0f);
						auto p11 = tileUV(useSideCol, useSideRow,1.0f,0.0f);
						auto p10 = tileUV(useSideCol, useSideRow,1.0f,1.0f);
						verts.insert(verts.end(), {
							(float)x +0.5f, (float)y +0.5f, (float)z +0.5f, p00.first, p00.second,
							(float)x +0.5f, (float)y -0.5f, (float)z +0.5f, p01.first, p01.second,
							(float)x +0.5f, (float)y -0.5f, (float)z -0.5f, p11.first, p11.second,
							(float)x +0.5f, (float)y +0.5f, (float)z -0.5f, p10.first, p10.second
						});
						indices.insert(indices.end(), { vertexCount, vertexCount +1, vertexCount +2, vertexCount +2, vertexCount +3, vertexCount });
						vertexCount +=4;
					}
				}

				// Top face (y+)
				{
					bool shouldRender = true;
					if (y +1 < SIZE) {
						if (blocks[x][y +1][z].type != BlockType::AIR) shouldRender = false;
					} else {
						auto it = allChunks.find(std::make_tuple(chunkX, chunkY +1, chunkZ));
						if (it == allChunks.end()) { /*missingNeighbor = true; break;*/ }
						if (it != allChunks.end() && it->second->blocks[x][0][z].type != BlockType::AIR) shouldRender = false;
					}
					if (shouldRender) {
						auto p00 = tileUV(useTopCol, useTopRow,0.0f,0.0f);
						auto p01 = tileUV(useTopCol, useTopRow,0.0f,1.0f);
						auto p11 = tileUV(useTopCol, useTopRow,1.0f,1.0f);
						auto p10 = tileUV(useTopCol, useTopRow,1.0f,0.0f);
						verts.insert(verts.end(), {
							(float)x -0.5f, (float)y +0.5f, (float)z -0.5f, p00.first, p00.second,
							(float)x -0.5f, (float)y +0.5f, (float)z +0.5f, p01.first, p01.second,
							(float)x +0.5f, (float)y +0.5f, (float)z +0.5f, p11.first, p11.second,
							(float)x +0.5f, (float)y +0.5f, (float)z -0.5f, p10.first, p10.second
						});
						indices.insert(indices.end(), { vertexCount, vertexCount +1, vertexCount +2, vertexCount +2, vertexCount +3, vertexCount });
						vertexCount +=4;
					}
				}

				// Bottom face (y-)
				{
					bool shouldRender = true;
					if (y -1 >=0) {
						if (blocks[x][y -1][z].type != BlockType::AIR) shouldRender = false;
					} else {
						auto it = allChunks.find(std::make_tuple(chunkX, chunkY -1, chunkZ));
						if (it == allChunks.end()) { /*missingNeighbor = true; break;*/ }
						if (it != allChunks.end() && it->second->blocks[x][SIZE -1][z].type != BlockType::AIR) shouldRender = false;
					}
					if (shouldRender) {
						auto p00 = tileUV(useBottomCol, useBottomRow,0.0f,0.0f);
						auto p10 = tileUV(useBottomCol, useBottomRow,1.0f,0.0f);
						auto p11 = tileUV(useBottomCol, useBottomRow,1.0f,1.0f);
						auto p01 = tileUV(useBottomCol, useBottomRow,0.0f,1.0f);
						verts.insert(verts.end(), {
							(float)x -0.5f, (float)y -0.5f, (float)z -0.5f, p00.first, p00.second,
							(float)x +0.5f, (float)y -0.5f, (float)z -0.5f, p10.first, p10.second,
							(float)x +0.5f, (float)y -0.5f, (float)z +0.5f, p11.first, p11.second,
							(float)x -0.5f, (float)y -0.5f, (float)z +0.5f, p01.first, p01.second
						});
						indices.insert(indices.end(), { vertexCount, vertexCount +1, vertexCount +2, vertexCount +2, vertexCount +3, vertexCount });
						vertexCount +=4;
					}
				}

			}
		}
	}

	if (!verts.empty()) {
	// Pack floats into rawVertices (bytes) to reduce memory usage until creating VBO
	rawVertices.resize(verts.size() * sizeof(float));
	std::memcpy(rawVertices.data(), verts.data(), rawVertices.size());

	if (vao) delete vao; if (vbo) delete vbo; if (ebo) delete ebo;
	vao = new VAO();
	vbo = new VBO(reinterpret_cast<GLfloat*>(rawVertices.data()), (GLsizeiptr)rawVertices.size());
	ebo = new EBO(reinterpret_cast<GLuint*>(indices.data()), indices.size() * sizeof(unsigned int));

	vao->bind();
	vao->linkVBO(*vbo,0,3, GL_FLOAT,5 * sizeof(float), (void*)0);
	vao->linkVBO(*vbo,1,2, GL_FLOAT,5 * sizeof(float), (void*)(3 * sizeof(float)));
	ebo->bind();
	vao->unbind();

	meshBuilt = true;
	} else {
	meshBuilt = false;
	}
}

void Chunk::draw(Shader& shader, Camera& camera) {
	if (!meshBuilt) {
		// rebuildMesh will be called from Planet
		return;
	}
	if (!vao) return;
		shader.use();

	glm::mat4 model = glm::translate(glm::mat4(1.0f), position * (float)SIZE);
	shader.setMat4("model", model);
	shader.setMat4("view", camera.GetViewMatrix());
	shader.setMat4("projection", camera.GetProjectionMatrix());
	vao->bind();
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT,0);
}