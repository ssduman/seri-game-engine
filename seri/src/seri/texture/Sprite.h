#pragma once

#include "seri/texture/Color.h"

#include <glm/glm.hpp>

#include <vector>

class Sprite {
public:
    Sprite() = default;

    ~Sprite() = default;

    std::vector<glm::vec2>& getTextureCoordinates() {
        return _textureCoordinates;
    }

private:
    Color _color;
    //Texture _texture;
    std::vector<glm::vec2> _textureCoordinates{
        glm::vec2{ 1.0f, 1.0f }, glm::vec2{ 1.0f, 0.0f }, glm::vec2{ 0.0f, 0.0f }, glm::vec2{ 0.0f, 1.0f }
    };

};
