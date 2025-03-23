#pragma once

#include <glm/glm.hpp>

#include <vector>

struct AuxiliaryStructsPrototype {
    static void forTriangle() {
        std::vector<glm::vec3> positions{ { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.5f, 0.0f }, { 0.5f, -0.5f, 0.0f } };
        std::vector<glm::vec2> texturePositions{ { 1.0f, 1.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 1.0f } };
        std::vector<unsigned int> indices{ 1, 2, 3 };

    }
};
