#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture::Texture(const std::string& texturePath) {
    init(texturePath);
}

Texture::~Texture() {
    glDeleteTextures(1, &_texture);
}

void Texture::init(const std::string& texturePath) {
    if (_texture != 0) {
        return;
    }

    glGenTextures(1, &_texture);
    glBindTexture(GL_TEXTURE_2D, _texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true);
    _image = stbi_load(texturePath.c_str(), &_width, &_height, &_components, 0);
    if (_image) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, _image);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(_image);
    }
}

void Texture::bind() {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _texture);
}

void Texture::unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}
