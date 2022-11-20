#pragma once

#include <glm/glm.hpp>

class Material {
public:
    Material() = default;

    ~Material() = default;

    glm::vec4& getAmbient() {
        return _ambient;
    }

    glm::vec4& getDiffuse() {
        return _diffuse;
    }

    glm::vec4& getSpecular() {
        return _specular;
    }

    glm::vec4& getEmissive() {
        return _emissive;
    }

    glm::vec4& getReflective() {
        return _reflective;
    }

    glm::vec4& getTransparent() {
        return _transparent;
    }

    float& getShininess() {
        return _shininess;
    }

private:
    glm::vec4 _ambient{};
    glm::vec4 _diffuse{};
    glm::vec4 _specular{};
    glm::vec4 _emissive{};
    glm::vec4 _reflective{};
    glm::vec4 _transparent{};
    float _shininess{ 0.0f };

};
