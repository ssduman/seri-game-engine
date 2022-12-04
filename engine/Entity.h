#pragma once

#include "Object.h"
#include "Color.h"
#include "Logger.h"
#include "Shader.h"
#include "ICamera.h"
#include "Texture.h"
#include "Transform.h"
#include "EntityType.h"
#include "ShaderManager.h"
#include "IEngineBackend.h"
#include "AuxiliaryStructs.h"
#include "OpenGLEngineBackend.h"

#include <GL/glew.h>

#include <memory>
#include <string>
#include <vector>

class Entity : public Object {
public:
    Entity() = delete;

    Entity(std::shared_ptr<ICamera> camera) : _camera(camera) {}

    Entity(Entity&& other) noexcept {
        _camera = std::move(other._camera);
        _shader = std::move(other._shader);
        _texture = std::move(other._texture);
        _color = std::move(other._color);
        _transform = std::move(other._transform);
        _entityType = std::move(other._entityType);
        _engineBackend = std::move(other._engineBackend);
        _shaderManager = std::move(other._shaderManager);

        other._shouldDeleteThis = false;
    }

    Entity(const Entity& other) = delete;

    Entity& operator=(Entity&& other) noexcept {
        _camera = std::move(other._camera);
        _shader = std::move(other._shader);
        _texture = std::move(other._texture);
        _color = std::move(other._color);
        _transform = std::move(other._transform);
        _entityType = std::move(other._entityType);
        _engineBackend = std::move(other._engineBackend);
        _shaderManager = std::move(other._shaderManager);

        other._shouldDeleteThis = false;

        return *this;
    }

    Entity& operator=(const Entity& other) = delete;

    ~Entity() override {
        if (_shouldDeleteThis) {
            //_engineBackend.release();
        }
    }

    void init() override {
        _shaderManager.initMVP(_camera);
    }

    void update() override {
        if (_camera) {
            _shader.use();
            _shader.setMat4("u_view", _camera->getView());
            _shader.disuse();
        }
    }

    Color& getColor() {
        return _color;
    }

    Shader& getShader() {
        return _shader;
    }

    Texture& getTexture() {
        return _texture;
    }

    Transform& getTransform() {
        return _transform;
    }

    ShaderManager& getShaderManager() {
        return _shaderManager;
    }

    OpenGLEngineBackend& getDrawer() {
        return _engineBackend;
    }

protected:
    std::shared_ptr<ICamera> _camera;
    Color _color;
    Shader _shader;
    Texture _texture;
    Transform _transform;
    ShaderManager _shaderManager{ _shader };
    OpenGLEngineBackend _engineBackend{ _shaderManager };
    EntityType _entityType{ EntityType::unknown };

    bool _shouldDeleteThis = true;

};
