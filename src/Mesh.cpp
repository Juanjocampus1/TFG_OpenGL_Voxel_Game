#include "Mesh.h"

Mesh::Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures) {
    Mesh::vertices = vertices;
    Mesh::indices = indices;
    Mesh::textures = textures;

    VAO.Bind();
    VBO VBO(vertices);
    EBO EBO(indices);

    VAO.LinkVBO(VBO, 0, 3, GL_FLOAT, 11 * sizeof(float), (void*)0);
    VAO.LinkVBO(VBO, 1, 3, GL_FLOAT, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    VAO.LinkVBO(VBO, 2, 2, GL_FLOAT, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    VAO.LinkVBO(VBO, 3, 3, GL_FLOAT, 11 * sizeof(float), (void*)(8 * sizeof(float)));

    VAO.Unbind();
    VBO.Unbind();
    EBO.Unbind();
}

void Mesh::Draw(Shader& shader, Camera& camera) {
    shader.use();
    VAO.Bind();

    unsigned int numDiffuse = 0;
    unsigned int numSpecular = 0;

    for (unsigned int i = 0; i < textures.size(); i++) {
        string num;
        string type = textures[i].type;
        if (type == "diffuse") {
            num = to_string(numDiffuse++);
        }
        else if (type == "specular") {
            num = to_string(numSpecular++);
        }
        textures[i].texUnit(shader, (type + num).c_str(), i);
        textures[i].Bind();
    }

    glUniform3f(glGetUniformLocation(shader.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
    camera.Matrix(shader, "camMatrix");

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}
