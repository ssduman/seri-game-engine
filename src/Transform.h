#pragma once

#include <glm/glm.hpp>

#include <vector>

class Transform {
public:
    Transform() = default;

    ~Transform() = default;

    std::vector<glm::vec3>& getTextureCoordinates() {
        return coordinates;
    }

private:
    glm::vec3 _position{ 0.0f, 0.0f, 0.0f };
    glm::vec3 _rotation{ 0.0f, 0.0f, 0.0f };
    glm::vec3 _scale{ 1.0f, 1.0f, 1.0f };
    std::vector<glm::vec3> coordinates;

};
