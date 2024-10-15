#include "texture.h"
#include <iostream>

using namespace std;

Texture::Texture(const char* image, GLenum texType, GLuint slot, GLenum format, GLenum pixelType) : ID(0) {
    // Load and create a texture    
    type = texType;
    // Flip the image vertically
    stbi_set_flip_vertically_on_load(true);
    // Load image, create texture and generate mipmaps
    int widthImg, heightImg, nrChannels;
    unsigned char* bytes = stbi_load(image, &widthImg, &heightImg, &nrChannels, 0);

    if (!bytes) {
        std::cout << "Failed to load texture: " << image << std::endl;
        return;
    }

    glGenTextures(1, &ID); // Inicializar ID
    glActiveTexture(GL_TEXTURE0 + slot);
	unit = slot;
    glBindTexture(texType, ID);

    // Set texture filtering parameters
    glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Set the texture wrapping parameters
    glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(texType, 0, GL_RGBA, widthImg, heightImg, 0, format, pixelType, bytes);
    glGenerateMipmap(texType);

    glBindTexture(texType, 0);
    stbi_image_free(bytes); // Liberar la memoria de la imagen
}


void Texture::texUnit(Shader& shader, const char* uniform, GLuint unit) {
    GLuint texUni = glGetUniformLocation(shader.ID, uniform);
    shader.use();
    glUniform1i(texUni, unit);
}

void Texture::Bind() {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(type, ID);
}

void Texture::Unbind() {
    glBindTexture(type, 0);
}

void Texture::Delete() {
    glDeleteTextures(1, &ID);
}
