#ifndef CUBE_H
#define CUBE_H

#include "../Renderer/VAO.h"
#include "../Renderer/VBO.h"
#include "../Renderer/EBO.h"
#include "../World/Block.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Cube {
public:
    Cube(float x, float y, float z, BlockType type);
    ~Cube();
    void Draw(class Shader& shader, class Camera& camera);

private:
    VAO* vao;
    VBO* vbo;
    EBO* ebo;
    float posX, posY, posZ;
    BlockType blockType;
};

#endif