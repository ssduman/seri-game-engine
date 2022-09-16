#pragma once

#include <glm/glm.hpp>

class Color {
public:
    Color() = default;

    Color(float c) {
        if (c > 1.0f) {
            c /= 255.0f;
        }
        _color.r = c;
        _color.g = c;
        _color.b = c;
    }

    Color(float r, float g, float b) {
        _color.r = r;
        _color.g = g;
        _color.b = b;
    }

    glm::vec3 getColor() {
        return _color;
    }

    glm::vec4 getColorRGBA() {
        return { _color, _alpha };
    }

    void setAlpha(float alpha) {
        _alpha = alpha;
    }

private:
    float _alpha = 1.0f;
    glm::vec3 _color{ 1.0f, 1.0f, 1.0f };

};
