#pragma once

#include "util/Util.h"
#include "core/Object.h"
#include "core/EntityType.h"
#include "texture/Color.h"
#include "texture/TextureBase.h"
#include "shader/ShaderLibrary.h"
#include "math/Transform.h"
#include "camera/CameraBase.h"
#include "rendering/render/RenderingManager.h"

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
		
		Transform _transform;
		EntityType _entityType{ EntityType::unknown };

	};
}
