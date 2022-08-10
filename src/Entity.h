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
    Entity() = default;

    virtual ~Entity() {}

    virtual void initShader(const std::string& vsCodePath, const std::string& fsCodePath) {
        _shader.init(vsCodePath, fsCodePath);
    }

    virtual void initTexture(const std::string& texturePath) {
        _useTexture = true;
        _shader.use();
        _shader.setBool("u_useTexture", true);
        _shader.disuse();
        _texture.init(texturePath);
    }

    virtual void setProperties(EntityProperties& entityProperties) {
        entityProperties.clipCoordinates = entityProperties.viewportCoordinates;
        entityProperties.vertexColors = entityProperties.colors;

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
    };

    inline Shader& getShader() {
        return _shader;
    }

protected:
    EntityType _entityType = EntityType::UNKNOWN;
    Shader _shader;
    Texture _texture;
    std::vector<GLfloat> _vertices;
    int _stride = 6;
    int _texStart = 0;
    bool _useTexture = false;

private:

};
