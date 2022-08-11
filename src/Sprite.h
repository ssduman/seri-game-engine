#pragma once

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
    std::vector<glm::vec2> _textureCoordinates{
        glm::vec2{ 1.0f, 1.0f }, glm::vec2{ 1.0f, 0.0f }, glm::vec2{ 0.0f, 0.0f }, glm::vec2{ 0.0f, 1.0f }
    };

};
