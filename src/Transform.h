#pragma once

#include <glm/glm.hpp>

#include <vector>

class Transform {
public:
    Transform() = default;

    ~Transform() = default;

    std::vector<glm::vec3>& getTextureCoordinates() {
        return viewportCoordinates;
    }

private:
    std::vector<glm::vec3> viewportCoordinates;

};
