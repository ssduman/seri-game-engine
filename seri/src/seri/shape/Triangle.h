#pragma once

#include "seri/core/Entity.h"

namespace seri
{
	class Triangle : public Entity
	{
	public:
		Triangle(std::shared_ptr<CameraBase> camera) : Entity(camera)
		{
			_entityType = EntityType::triangle;
		}

		~Triangle() override = default;

		void Render() override
		{
			_shader->Bind();
			_texture->Bind();
		}

	private:

	};
}
