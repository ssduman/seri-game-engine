#pragma once

#include "seri/core/Object.h"
#include "seri/core/EntityType.h"
#include "seri/texture/Color.h"
#include "seri/texture/Texture.h"
#include "seri/shader/Shader.h"
#include "seri/shader/ShaderManager.h"
#include "seri/math/Transform.h"
#include "seri/camera/ICamera.h"
#include "seri/logging/Logger.h"
#include "seri/renderer/OpenGLEngineBackend.h"

#include <memory>
#include <string>
#include <vector>

class Entity : public Object
{
public:
	Entity() = delete;

	Entity(std::shared_ptr<ICamera> camera) : _camera(camera) {}

	Entity(Entity&& other) = default;

	Entity(const Entity& other) = delete;

	Entity& operator=(Entity&& other) = default;

	Entity& operator=(const Entity& other) = delete;

	~Entity() override
	{
		_engineBackend.release();
	}

	void init() override
	{
		ShaderManager::GetInstance().initMVP(_shader, _camera);
	}

	void update() override
	{
		ShaderManager::GetInstance().setView(_shader, _camera->getView());
		ShaderManager::GetInstance().setProjection(_shader, _camera->getProjection());
	}

	Color& getColor()
	{
		return _color;
	}

	Shader& getShader()
	{
		return _shader;
	}

	Texture& getTexture()
	{
		return _texture;
	}

	Transform& getTransform()
	{
		return _transform;
	}

	OpenGLEngineBackend& getDrawer()
	{
		return _engineBackend;
	}

protected:
	std::shared_ptr<ICamera> _camera;
	Color _color;
	Shader _shader;
	Texture _texture;
	Transform _transform;
	OpenGLEngineBackend _engineBackend{};
	EntityType _entityType{ EntityType::unknown };

};
