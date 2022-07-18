#pragma once

#include <GL/glew.h>

class TextureManager {
private:
    unsigned int ID;

public:
    TextureManager(const char* path);
    ~TextureManager();
    void bind();
    void unbind();
};
