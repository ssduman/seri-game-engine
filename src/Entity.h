#pragma once

#include "Object.h"
#include "Window.h"
#include "Shader.h"
#include "Texture.h"

#include <glm/glm.hpp>

#include <string>
#include <vector>

enum class EntityType {
    POINT,
    LINE,
    TRIANGLE,
    RECTANGLE,
    CIRCLE,
    UNKNOWN,
};

struct EntityProperties {
    std::vector<glm::vec3> viewportCoordinates;
    std::vector<glm::vec3> colors;
    std::vector<glm::vec2> textureCoordinates = {
        glm::vec2{ 1.0f, 1.0f, }, glm::vec2{ 1.0f, 0.0f, }, glm::vec2{ 0.0f, 0.0f, }, glm::vec2{ 0.0f, 1.0f, },
    };

    // GL_POINTS, GL_LINES, GL_LINE_STRIP, GL_LINE_LOOP, GL_TRIANGLES, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN,
    int drawMode = GL_TRIANGLES;

    std::vector<glm::vec3> clipCoordinates;
    std::vector<glm::vec3> vertexColors;
};

class Entity : public Object {
public:
    Entity(const WindowProperties& windowProperties) : _windowProperties(windowProperties) {}

    virtual void initShader(const std::string& vsCodePath, const std::string& fsCodePath) {
        _shader.init(vsCodePath, fsCodePath);
    }

    virtual void initTexture(const std::string& texturePath) {
        _texture.init(texturePath);
    }

    virtual void setProperties(EntityProperties& entityProperties) {
        entityProperties.clipCoordinates = entityProperties.viewportCoordinates;
        entityProperties.vertexColors = entityProperties.colors;

        //for (auto& coordinate : entityProperties.clipCoordinates) {
        //    viewportToClipCoordinate(coordinate);
        //}

        //for (auto& color : entityProperties.vertexColors) {
        //    mapRGBColor(color);
        //}

        auto vertexCount = entityProperties.clipCoordinates.size();
        for (int i = 0; i < vertexCount; i++) {
            _vertices.push_back(entityProperties.clipCoordinates[i].x);
            _vertices.push_back(entityProperties.clipCoordinates[i].y);
            _vertices.push_back(entityProperties.clipCoordinates[i].z);

            _vertices.push_back(entityProperties.vertexColors[i].x);
            _vertices.push_back(entityProperties.vertexColors[i].y);
            _vertices.push_back(entityProperties.vertexColors[i].z);
        }

        for (int i = 0; i < 4; i++) {
            _vertices.push_back(entityProperties.textureCoordinates[i].x);
            _vertices.push_back(entityProperties.textureCoordinates[i].y);
        }

        _texStart = static_cast<int>(_vertices.size()) - 8;

        //for (int i = 0; i < vertexCount; i++) {
        //    std::cout
        //        << "["
        //        << _vertices[i * 6 + 0] << ", " << _vertices[i * 6 + 1] << ", " << _vertices[i * 6 + 2]
        //        << "], ["
        //        << _vertices[i * 6 + 3] << ", " << _vertices[i * 6 + 4] << ", " << _vertices[i * 6 + 5]
        //        << "]\n";
        //}
    };

    inline Shader& getShader() {
        return _shader;
    }

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
        return map(x, minViewportValue, (float)_windowProperties.windowWidth, clipWorldRange[0], clipWorldRange[1]);
    }

    inline float mapPositionHeight(float x) {
        return map(x, minViewportValue, (float)_windowProperties.windowHeight, clipWorldRange[0], clipWorldRange[1]);
    }

    inline float mapColor(float x) {
        return map(x, colorRGBRange[0], colorRGBRange[1], colorRange[0], colorRange[1]);
    }

    inline float map(float x, float in_min, float in_max, float out_min, float out_max) {
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }

    EntityType _EntityType = EntityType::UNKNOWN;
    const WindowProperties& _windowProperties;
    Shader _shader;
    Texture _texture;
    std::vector<GLfloat> _vertices;
    int _stride = 6;
    int _texStart = 0;

private:
    float minViewportValue = 0.0f;
    std::vector<float> colorRange{ 0.0f, 1.0f };
    std::vector<float> colorRGBRange{ 0.0f, 255.0f };
    std::vector<float> clipWorldRange{ -1.0f, 1.0f };

};
