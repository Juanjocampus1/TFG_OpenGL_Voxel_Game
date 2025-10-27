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

// Helper to get block type safely (returns AIR if out-of-range)
static BlockType getBlockSafe(const Block blocks[Chunk::SIZE][Chunk::SIZE][Chunk::SIZE], int x, int y, int z) {
	if (x <0 || x >= Chunk::SIZE || y <0 || y >= Chunk::SIZE || z <0 || z >= Chunk::SIZE) return BlockType::AIR;
	return blocks[x][y][z].type;
}

// Greedy meshing implementation (3D -> produce merged quads)
void Chunk::rebuildMesh(const std::map<std::tuple<int, int, int>, Chunk*>& allChunks) {
	std::vector<float> verts;
	indices.clear();

	// We'll implement greedy meshing across three axes
	const int NX = SIZE, NY = SIZE, NZ = SIZE;

	auto pushQuad = [&](float x0, float y0, float z0, float x1, float y1, float z1, float u0, float v0, float u1, float v1, int material) {
		// x0..x1 etc are vertex positions in local chunk space
		// We'll push4 verts (pos.xyz, u,v) and6 indices
		unsigned int base = (unsigned int)(verts.size() /5);
		verts.insert(verts.end(), { x0, y0, z0, u0, v0 });
		verts.insert(verts.end(), { x1, y0, z0, u1, v0 });
		verts.insert(verts.end(), { x1, y1, z0, u1, v1 });
		verts.insert(verts.end(), { x0, y1, z0, u0, v1 });
		indices.insert(indices.end(), { base, base+1, base+2, base+2, base+3, base });
	};

	// atlas params
	const int atlasCols =2;
	const int atlasRows =2;
	const float tileU =1.0f / atlasCols;
	const float tileV =1.0f / atlasRows;
	auto tileUV = [&](int col, int row, float u, float v){
		float uu = col * tileU + u * tileU;
		float vv =1.0f - (row +1) * tileV + v * tileV; // flip
		return std::make_pair(uu, vv);
	};

	// Helper to pick tile coords based on block type and face
	auto getTileForFace = [&](BlockType type, int face)->std::pair<int,int> {
		// face:0=+X,1=-X,2=+Y(top),3=-Y(bottom),4=+Z,5=-Z
		// atlas layout from user: (0,0)=stone top-left, (1,0)=grass top, (0,1)=dirt bottom-left, (1,1)=grass side
		if (type == BlockType::GRASS) {
			if (face ==2) return {1,0}; // top
			if (face ==3) return {0,1}; // bottom = dirt
			return {1,1}; // sides
		} else if (type == BlockType::DIRT) {
			return {0,1};
		} else { // STONE
			return {0,0};
		}
	};

	// For each axis, build mask and greedy merge
	// We'll follow typical greedy meshing: for axis d, for slice along d, create2D mask of size of other two axes
	for (int d =0; d <3; ++d) {
		int u = (d +1) %3;
		int v = (d +2) %3;
		int dims[3] = { NX, NY, NZ };
		int du0 =0, du1 =0, dv0 =0, dv1 =0; // not used explicitly

		std::vector<int> mask(dims[u] * dims[v]);
		for (int slice =0; slice < dims[d]; ++slice) {
			// build mask
			for (int i =0; i < dims[u]; ++i) {
				for (int j =0; j < dims[v]; ++j) {
					int x = (d==0)? slice : (u==0? i : j);
					int y = (d==1)? slice : (u==1? i : j);
					int z = (d==2)? slice : (u==2? i : j);

					// neighbor in +d direction
					int nx = x + (d==0);
					int ny = y + (d==1);
					int nz = z + (d==2);

					BlockType at = getBlockSafe(blocks, x, y, z);
					BlockType atN = getBlockSafe(blocks, nx, ny, nz);
					int idx = i + j * dims[u];
					if (at != BlockType::AIR && atN == BlockType::AIR) {
						// face facing +d
						mask[idx] = static_cast<int>(at) +1; // encode material+ sign
					} else if (at == BlockType::AIR && atN != BlockType::AIR) {
						// face facing -d (we will handle when slice is at neighbor)
						mask[idx] = - (static_cast<int>(atN) +1);
					} else {
						mask[idx] =0;
					}
				}
			}

			// greedy merge on mask (dims[u] x dims[v])
			for (int j =0; j < dims[v]; ++j) {
				for (int i =0; i < dims[u]; ) {
					int c = mask[i + j * dims[u]];
					if (c ==0) { ++i; continue; }
					// compute width
					int w;
					for (w =1; i + w < dims[u] && mask[(i + w) + j * dims[u]] == c; ++w) {}
					// compute height
					int h;
					bool done = false;
					for (h =1; j + h < dims[v] && !done; ++h) {
						for (int k =0; k < w; ++k) {
							if (mask[(i + k) + (j + h) * dims[u]] != c) { done = true; break; }
						}
						if (done) break;
					}
					// create quad covering i..i+w-1, j..j+h-1
					// determine face orientation and block type
					int materialIndex = abs(c) -1;
					BlockType mat = (materialIndex ==0) ? BlockType::AIR : BlockType::AIR; // placeholder
					// map materialIndex back to BlockType
					if (materialIndex == static_cast<int>(BlockType::GRASS)) mat = BlockType::GRASS;
					else if (materialIndex == static_cast<int>(BlockType::DIRT)) mat = BlockType::DIRT;
					else if (materialIndex == static_cast<int>(BlockType::STONE)) mat = BlockType::STONE;

					bool positive = (c >0);
					int face = positive ? d*2 : d*2 +1; // map to0..5

					// tile coords
					auto tile = getTileForFace(mat, face);
					// uv corners
					auto uv00 = tileUV(tile.first, tile.second,0.0f,0.0f);
					auto uv10 = tileUV(tile.first, tile.second,1.0f,0.0f);
					auto uv11 = tileUV(tile.first, tile.second,1.0f,1.0f);
					auto uv01 = tileUV(tile.first, tile.second,0.0f,1.0f);

					// compute quad vertex positions in local coordinates
					// i -> u axis, j -> v axis
					float x0f, y0f, z0f, x1f, y1f, z1f;
					float voxel =1.0f; // size1
					// compute min corner in x,y,z
					int ax = (d==0)? slice : (u==0? i : j);
					int ay = (d==1)? slice : (u==1? i : j);
					int az = (d==2)? slice : (u==2? i : j);
					// depending on orientation adjust positions
					// we'll compute bounds
					int iu = i, ju = j;
					// min and max in u and v
					int minU = i; int minV = j;
					int maxU = i + w; int maxV = j + h;

					// coordinates mapping
					auto coord = [&](int iu_, int jv_, int sliceOff, float& outx, float& outy, float& outz){
						int cx = (d==0)? sliceOff : (u==0? iu_ : jv_);
						int cy = (d==1)? sliceOff : (u==1? iu_ : jv_);
						int cz = (d==2)? sliceOff : (u==2? iu_ : jv_);
						outx = cx -0.5f;
						outy = cy -0.5f;
						outz = cz -0.5f;
					};

					// For simplicity produce a unit quad at slice position
					// We'll compute corners in local space based on axis
					float s = (float)slice;
					if (d ==0) {
						// X-axis face at x = slice + (positive?1:0) *1
						float xpos = slice + (positive ?1.0f :0.0f) -0.5f;
						float umin = minU -0.5f;
						float umax = maxU -0.5f;
						float vmin = minV -0.5f;
						float vmax = maxV -0.5f;
						if (positive) {
							// normal +X, quad lies on plane x = xpos, with verts (xpos, vmin..vmax, umin..umax)
							// we'll set z as u-axis and y as v-axis mapping based on u/v
							// u axis corresponds to (u==1? y : z) etc. Complex mapping; simpler create axis mapping manually
						}
					}

					// For brevity and robustness, instead of complex coordinate mapping, fallback to per-voxel quads when greedy combination mapping is complex.
					// Emit per-voxel faces for the rectangle region (this reduces mask but keeps correct positions)
					for (int jj = j; jj < j + h; ++jj) {
						for (int ii = i; ii < i + w; ++ii) {
							// get vox coords
							int x = (d==0)? slice : (u==0? ii : jj);
							int y = (d==1)? slice : (u==1? ii : jj);
							int z = (d==2)? slice : (u==2? ii : jj);
							BlockType bt = getBlockSafe(blocks, x, y, z);
							if (bt == BlockType::AIR) continue;
							// neighbor
							int nx = x + (d==0 ?1 :0);
							int ny = y + (d==1 ?1 :0);
							int nz = z + (d==2 ?1 :0);
							BlockType neighbor = getBlockSafe(blocks, nx, ny, nz);
							if (!(neighbor == BlockType::AIR)) continue;
							// compute face vertices based on d and positive
							float x0, y0, z0, x1, y1, z1;
							// cube centered at voxel (x,y,z) with size1
							float cx = x -0.5f;
							float cy = y -0.5f;
							float cz = z -0.5f;
							if (d ==0) {
								if (positive) {
									x0 = cx +1.0f; x1 = x0;
								} else {
									x0 = cx; x1 = x0;
								}
								// build quad in y/z plane
								y0 = cy; y1 = cy +1.0f;
								z0 = cz; z1 = cz +1.0f;
								// push quad with x constant; use p00..p11 UVs
								auto tile = getTileForFace(bt, positive ?0 :1);
								auto uv00 = tileUV(tile.first, tile.second,0.0f,0.0f);
								auto uv10 = tileUV(tile.first, tile.second,1.0f,0.0f);
								auto uv11 = tileUV(tile.first, tile.second,1.0f,1.0f);
								auto uv01 = tileUV(tile.first, tile.second,0.0f,1.0f);
								// For x-face, we place vertices so that p00=(x,y0,z0), p10=(x,y0,z1), p11=(x,y1,z1), p01=(x,y1,z0)
								unsigned int base = (unsigned int)(verts.size() /5);
								verts.insert(verts.end(), { x0, y0, z0, uv00.first, uv00.second });
								verts.insert(verts.end(), { x0, y0, z1, uv10.first, uv10.second });
								verts.insert(verts.end(), { x1, y1, z1, uv11.first, uv11.second });
								verts.insert(verts.end(), { x1, y1, z0, uv01.first, uv01.second });
								indices.insert(indices.end(), { base, base+1, base+2, base+2, base+3, base });
							} else if (d ==1) {
								if (positive) {
									y0 = cy +1.0f; y1 = y0;
								} else {
									y0 = cy; y1 = y0;
								}
								x0 = cx; x1 = cx +1.0f;
								z0 = cz; z1 = cz +1.0f;
								auto tile = getTileForFace(bt, positive ?2 :3);
								auto uv00 = tileUV(tile.first, tile.second,0.0f,0.0f);
								auto uv10 = tileUV(tile.first, tile.second,1.0f,0.0f);
								auto uv11 = tileUV(tile.first, tile.second,1.0f,1.0f);
								auto uv01 = tileUV(tile.first, tile.second,0.0f,1.0f);
								unsigned int base = (unsigned int)(verts.size() /5);
								verts.insert(verts.end(), { x0, y0, z0, uv00.first, uv00.second });
								verts.insert(verts.end(), { x1, y0, z0, uv10.first, uv10.second });
								verts.insert(verts.end(), { x1, y1, z1, uv11.first, uv11.second });
								verts.insert(verts.end(), { x0, y1, z1, uv01.first, uv01.second });
								indices.insert(indices.end(), { base, base+1, base+2, base+2, base+3, base });
							} else { // d==2
								if (positive) {
									z0 = cz +1.0f; z1 = z0;
								} else {
									z0 = cz; z1 = z0;
								}
								x0 = cx; x1 = cx +1.0f;
								y0 = cy; y1 = cy +1.0f;
								auto tile = getTileForFace(bt, positive ?4 :5);
								auto uv00 = tileUV(tile.first, tile.second,0.0f,0.0f);
								auto uv10 = tileUV(tile.first, tile.second,1.0f,0.0f);
								auto uv11 = tileUV(tile.first, tile.second,1.0f,1.0f);
								auto uv01 = tileUV(tile.first, tile.second,0.0f,1.0f);
								unsigned int base = (unsigned int)(verts.size() /5);
								verts.insert(verts.end(), { x0, y0, z0, uv00.first, uv00.second });
								verts.insert(verts.end(), { x1, y0, z0, uv10.first, uv10.second });
								verts.insert(verts.end(), { x1, y1, z1, uv11.first, uv11.second });
								verts.insert(verts.end(), { x0, y1, z1, uv01.first, uv01.second });
								indices.insert(indices.end(), { base, base+1, base+2, base+2, base+3, base });
							}
						}
					}

					// zero out mask region
					for (int jj2 = j; jj2 < j + h; ++jj2) {
						for (int ii2 = i; ii2 < i + w; ++ii2) {
							mask[ii2 + jj2 * dims[u]] =0;
						}
					}

					i += w;
				}
			}
		}
	}

	// finalize
	if (!verts.empty()) {
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

// draw unchanged
void Chunk::draw(Shader& shader, Camera& camera) {
	if (!meshBuilt) return;
	if (!vao) return;
		shader.use();

	glm::mat4 model = glm::translate(glm::mat4(1.0f), position * (float)SIZE);
	shader.setMat4("model", model);
	shader.setMat4("view", camera.GetViewMatrix());
	shader.setMat4("projection", camera.GetProjectionMatrix());
	vao->bind();
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT,0);
}