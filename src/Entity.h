#pragma once

#include "Object.h"

#include <glm/glm.hpp>

#include <string>
#include <vector>

class Entity : public Object {
public:
    Entity(int windowWidth, int windowHeight) : _windowWidth(windowWidth), _windowHeight(windowHeight) {}

    virtual void initShader(const std::string& vs_path, const std::string& fs_path) = 0;

    virtual void setProperties(std::vector<glm::vec3>& viewportCoordinates, std::vector<glm::vec3>& colors) = 0;

protected:
    void viewportToClipCoordinate(glm::vec3& viewportCoordinate) {
        viewportCoordinate.x = mapPositionWidth(viewportCoordinate.x);
        viewportCoordinate.y = mapPositionHeight(viewportCoordinate.y);
    }

    void mapRGBColor(glm::vec3& colors) {
        colors.x = mapColor(colors.x);
        colors.y = mapColor(colors.y);
        colors.z = mapColor(colors.z);
    }

    inline float mapPositionWidth(float x) {
        return map(x, minViewportValue, (float)_windowWidth, clipWorldRange[0], clipWorldRange[1]);
    }

    inline float mapPositionHeight(float x) {
        return map(x, minViewportValue, (float)_windowHeight, clipWorldRange[0], clipWorldRange[1]);
    }

    inline float mapColor(float x) {
        return map(x, colorRGBRange[0], colorRGBRange[1], colorRange[0], colorRange[1]);
    }

    inline float map(float x, float in_min, float in_max, float out_min, float out_max) {
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }

    int _windowWidth, _windowHeight;

private:
    float minViewportValue = 0.0f;
    std::vector<float> colorRange{ 0.0f, 1.0f };
    std::vector<float> colorRGBRange{ 0.0f, 255.0f };
    std::vector<float> clipWorldRange{ -1.0f, 1.0f };

};
