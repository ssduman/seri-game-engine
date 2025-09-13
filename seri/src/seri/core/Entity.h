#pragma once

#include "seri/core/Object.h"
#include "seri/core/EntityType.h"
#include "seri/texture/Color.h"
#include "seri/texture/TextureBase.h"
#include "seri/shader/ShaderBase.h"
#include "seri/shader/ShaderLibrary.h"
#include "seri/math/Transform.h"
#include "seri/camera/CameraBase.h"
#include "seri/logging/Logger.h"
#include "seri/renderer/RendererBackendOpenGL.h"

#include <memory>
#include <string>
#include <vector>

namespace seri
{
	class Entity : public Object
	{
	public:
		Entity() = delete;

		Entity(std::shared_ptr<CameraBase>& camera) : _camera(camera)
		{
			_shader = ShaderBase::Create();
			_texture = TextureBase::Create();
		}

		Entity(Entity&& other) = default;

		Entity(const Entity& other) = delete;

		Entity& operator=(Entity&& other) = default;

		Entity& operator=(const Entity& other) = delete;

		~Entity() override
		{
		}

		void Init() override
		{
			ShaderLibrary::SetMVP(_shader, _camera);
		}

		void Update() override
		{
			ShaderLibrary::SetView(_shader, _camera->GetView());
			ShaderLibrary::SetProjection(_shader, _camera->GetProjection());
		}

		Color& GetColor()
		{
			return _color;
		}

		std::shared_ptr<ShaderBase>& GetShader()
		{
			return _shader;
		}

		std::shared_ptr<TextureBase>& GetTexture()
		{
			return _texture;
		}

		Transform& GetTransform()
		{
			return _transform;
		}

	protected:
		std::shared_ptr<CameraBase> _camera;
		std::shared_ptr<ShaderBase> _shader;
		std::shared_ptr<TextureBase> _texture;
		Color _color;
		Transform _transform;
		EntityType _entityType{ EntityType::unknown };

	};
}
