#pragma once

#include "seri/core/Entity.h"

namespace seri
{
	class Circle : public Entity
	{
	public:
		Circle(std::shared_ptr<CameraBase> camera) : Entity(camera)
		{
			_entityType = EntityType::circle;
		}

		~Circle() override = default;

		void Render() override
		{
			_shader->Bind();
			_texture->Bind();
		}

	private:

	};
}
