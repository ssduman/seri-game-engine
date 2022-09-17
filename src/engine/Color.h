#pragma once

#include <glm/glm.hpp>

class Color {
public:
    Color() = default;

    Color(float c) {
        normalize(c);

        r = c;
        g = c;
        b = c;
    }

    Color(float _r, float _g, float _b) {
        normalize(_r);
        normalize(_g);
        normalize(_b);

        r = _r;
        g = _g;
        b = _b;
    }

    Color(float _r, float _g, float _b, float _a) {
        normalize(_r);
        normalize(_g);
        normalize(_b);
        normalize(_a);

        r = _r;
        g = _g;
        b = _b;
        a = _a;
    }

    glm::vec3 getColor() {
        return { r, g, b };
    }

    glm::vec4 getColorRGBA() {
        return { r, g, b, a };
    }

    float r = 1.0f;
    float g = 1.0f;
    float b = 1.0f;
    float a = 1.0f;

private:
    void normalize(float& c) {
        if (c > 1.0f) {
            c /= 255.0f;
        }
    }

};
