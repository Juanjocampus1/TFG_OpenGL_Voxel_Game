#ifndef MESH_CLASS_H
#define MESH_CLASS_H

#include <string>
#include <vector>
#include "vao.h"
#include "ebo.h"
#include "camera.h"
#include "Texture.h"

using namespace std;

class Mesh {
    public:
        vector<Vertex> vertices;
        vector<GLuint> indices;
        vector<Texture> textures;

        VAO VAO;

        Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures);

        void Draw(Shader& shader, Camera& camera);
};

#endif
