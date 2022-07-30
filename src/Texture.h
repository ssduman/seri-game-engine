#pragma once

#include <GL/glew.h>

#include <string>

class Texture {
public:
    Texture() = default;

    Texture(const std::string& texturePath);

    ~Texture();

    void init(const std::string& texturePath);

    void bind();

    void unbind();

private:
    int _width = 0;
    int _height = 0;
    int _components = 0;
    unsigned int _texture = 0;
    unsigned char* _image = nullptr;

};
