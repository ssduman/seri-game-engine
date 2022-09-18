#pragma once

#include "Logger.h"

#include <GL/glew.h>
#include <stb_image.h>

#include <string>

class Texture {
public:
    Texture() {
        LOGGER(info, "texture init succeeded");
    }

    ~Texture() {
        glDeleteTextures(1, &_tex);

        LOGGER(info, "texture delete succeeded");
    }

    void init(const std::string& texturePath) {
        glGenTextures(1, &_tex);
        glBindTexture(GL_TEXTURE_2D, _tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        setTextureFlip(true);

        int width, height, components;
        if (auto image = loadTexture(texturePath, width, height, components, 0)) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
            glGenerateMipmap(GL_TEXTURE_2D);
            unloadTexture(image);
        }
    }

    void bind() {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _tex);
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
    unsigned int _tex = 0;

};
