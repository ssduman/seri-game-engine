#pragma once

#include <GL/glew.h>
#include <stb_image.h>

#include <string>

class Texture {
public:
    Texture() = default;

    Texture(const std::string& texturePath) {
        init(texturePath);
    }

    ~Texture() {
        glDeleteTextures(1, &_texture);
    }

    void init(const std::string& texturePath) {
        if (_texture != 0) {
            return;
        }

        glGenTextures(1, &_texture);
        glBindTexture(GL_TEXTURE_2D, _texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        setTextureFlip(true);
        _image = loadTexture(texturePath, _width, _height, _components, 0);
        if (_image) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, _image);
            glGenerateMipmap(GL_TEXTURE_2D);
            unloadTexture(_image);
        }
    }

    void bind() {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _texture);
    }

    void unbind() {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    static unsigned char* loadTexture(const std::string& texturePath, int& width, int& height, int& components, int req_comp) {
        return stbi_load(texturePath.c_str(), &width, &height, &components, req_comp);
    }

    static void setTextureFlip(bool flip) {
        stbi_set_flip_vertically_on_load(flip);
    }

    static void unloadTexture(unsigned char* image) {
        stbi_image_free(image);
    }

private:
    int _width = 0;
    int _height = 0;
    int _components = 0;
    unsigned int _texture = 0;
    unsigned char* _image = nullptr;

};
