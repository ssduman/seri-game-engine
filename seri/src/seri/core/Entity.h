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
#include "seri/renderer/OpenGLRendererBackend.h"

#include <memory>
#include <string>
#include <vector>

namespace seri
{
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
			_engineBackend.Release();
		}

		void Init() override
		{
			ShaderManager::GetInstance().InitMVP(_shader, _camera);
		}

		void Update() override
		{
			ShaderManager::GetInstance().SetView(_shader, _camera->GetView());
			ShaderManager::GetInstance().SetProjection(_shader, _camera->GetProjection());
		}

		Color& GetColor()
		{
			return _color;
		}

		Shader& GetShader()
		{
			return _shader;
		}

		Texture& GetTexture()
		{
			return _texture;
		}

		Transform& GetTransform()
		{
			return _transform;
		}

		OpenGLRendererBackend& GetDrawer()
		{
			return _engineBackend;
		}

	protected:
		std::shared_ptr<ICamera> _camera;
		Color _color;
		Shader _shader;
		Texture _texture;
		Transform _transform;
		OpenGLRendererBackend _engineBackend{};
		EntityType _entityType{ EntityType::unknown };

	};
}
