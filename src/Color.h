#pragma once

#include <glm/glm.hpp>

#include <vector>

class Color {
public:
    Color() = default;

    ~Color() = default;

    std::vector<glm::vec3>& getColors() {
        return _colors;
    }

private:
    std::vector<glm::vec3> _colors;
};
