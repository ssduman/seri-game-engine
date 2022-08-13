#pragma once

#include "Object.h"
#include "Window.h"
#include "Shader.h"
#include "Texture.h"
#include "Transform.h"

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
    std::vector<glm::vec3> coordinates;
    std::vector<glm::vec3> colors;
    int drawMode = GL_TRIANGLES;

    std::vector<glm::vec2> textureCoordinates() {
        return { glm::vec2{ 1.0f, 1.0f }, glm::vec2{ 1.0f, 0.0f }, glm::vec2{ 0.0f, 0.0f }, glm::vec2{ 0.0f, 1.0f } };
    };
};

class Entity : public Object {
public:
    Entity() = default;

    virtual ~Entity() {}

    virtual void initShader(const std::string& vsCodePath, const std::string& fsCodePath) {
        _shader.init(vsCodePath, fsCodePath);
        _shader.use();
        _shader.setMat4("u_transform", glm::mat4(1.0f));
        _shader.disuse();
    }

    virtual void initTexture(const std::string& texturePath) {
        _useTexture = true;
        _shader.use();
        _shader.setBool("u_useTexture", true);
        _shader.disuse();
        _texture.init(texturePath);
    }

    virtual void setProperties(EntityProperties& entityProperties) {
        auto vertexCount = entityProperties.coordinates.size();
        for (int i = 0; i < vertexCount; i++) {
            _vertices.push_back(entityProperties.coordinates[i].x);
            _vertices.push_back(entityProperties.coordinates[i].y);
            _vertices.push_back(entityProperties.coordinates[i].z);

            _vertices.push_back(entityProperties.colors[i].x);
            _vertices.push_back(entityProperties.colors[i].y);
            _vertices.push_back(entityProperties.colors[i].z);
        }

        for (int i = 0; i < 4; i++) {
            _vertices.push_back(entityProperties.textureCoordinates()[i].x);
            _vertices.push_back(entityProperties.textureCoordinates()[i].y);
        }

        _texStart = static_cast<int>(_vertices.size()) - 8;
    };

    inline Transform& getTransform() {
        return _transform;
    }

    inline Shader& getShader() {
        return _shader;
    }

protected:
    Transform _transform;
    EntityType _entityType = EntityType::UNKNOWN;
    Shader _shader;
    Texture _texture;
    std::vector<GLfloat> _vertices;
    int _stride = 6;
    int _texStart = 0;
    bool _useTexture = false;

private:

};
