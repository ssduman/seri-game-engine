#pragma once

#include "seri/core/Entity.h"

namespace seri
{
	class Rectangle : public Entity
	{
	public:
		Rectangle(std::shared_ptr<CameraBase> camera) : Entity(camera)
		{
			_entityType = EntityType::rectangle;
		}

		~Rectangle() override = default;

		void Render() override
		{
			_shader->Bind();
			_texture->Bind();
		}

	private:

	};
}
