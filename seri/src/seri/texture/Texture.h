#pragma once

#include "seri/logging/Logger.h"

#include <glad/gl.h>
#include <stb_image.h>

class Texture {
public:
    Texture() = default;

    Texture(Texture&& other) = default;

    Texture(Texture& other) = default;

    Texture& operator=(Texture&& other) = default;

    Texture& operator=(Texture& other) = default;

    ~Texture() {
        unbind();
        del();
    }

    void init(const std::string& texturePath) {
        int width, height, components;
        if (auto image = loadTexture(texturePath, width, height, components, 0)) {
            init(image, width, height, components);
        }
        else {
            LOGGER(error, "init texture '" << texturePath << "' failed: " << stbi_failure_reason());
        }
    }

    void init(const void* data, unsigned int size) {
        int width, height, components;
        if (auto image = loadTexture(data, size, width, height, components, 0)) {
            init(image, width, height, components);
        }
        else {
            LOGGER(error, "init texture from buffer with size " << size << " failed: " << stbi_failure_reason());
        }
    }

    void bind() {
        glBindTexture(GL_TEXTURE_2D, _tex);
    }

    void unbind() {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    static unsigned char* loadTexture(const std::string& texturePath, int& width, int& height, int& components, int req_comp) {
        return stbi_load(texturePath.c_str(), &width, &height, &components, req_comp);
    }

    static unsigned char* loadTexture(const void* data, unsigned int size, int& width, int& height, int& components, int req_comp) {
        return stbi_load_from_memory((const stbi_uc*)data, size, &width, &height, &components, req_comp);
    }

    static void setTextureFlip(bool flip) {
        stbi_set_flip_vertically_on_load(flip);
    }

    static void unloadTexture(unsigned char* image) {
        stbi_image_free(image);
    }

private:
    void init(unsigned char* image, int width, int height, int components) {
        GLenum format = GL_RED;
        if (components == 3) {
            format = GL_RGB;
        }
        if (components == 4) {
            format = GL_RGBA;
        }

        generate();
        bind();

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);

        unbind();
        unloadTexture(image);
    }

    void generate() {
        glGenTextures(1, &_tex);
    }

    void del() {
        glDeleteTextures(1, &_tex);
        _tex = 0;
    }

    unsigned int _tex{ 0 };

};
