#pragma once

#include "Object.h"
#include "EntityType.h"
#include "AuxiliaryStructs.h"
#include "../math/Transform.h"
#include "../camera/ICamera.h"
#include "../logging/Logger.h"
#include "../shader/Shader.h"
#include "../shader/ShaderManager.h"
#include "../texture/Color.h"
#include "../texture/Texture.h"
#include "../renderer/IEngineBackend.h"
#include "../renderer/OpenGLEngineBackend.h"

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
            _engineBackend.release();
        }
    }

    void init() override {
        _shaderManager.initMVP(_camera);
    }

    void update() override {
        if (_camera) {
            _shaderManager.setView(_camera->getView());
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
