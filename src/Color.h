#pragma once

#include <glm/glm.hpp>

#include <vector>

class Color {
public:
    Color() = default;

    ~Color() = default;

    glm::vec3& getColor() {
        return _color;
    }

private:
    glm::vec3 _color{ 1.0f, 1.0f, 1.0f };
};
