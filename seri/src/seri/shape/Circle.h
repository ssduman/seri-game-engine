#pragma once

#include "seri/core/Entity.h"

namespace seri
{
	class Circle : public Entity
	{
	public:
		Circle(std::shared_ptr<ICamera> camera) : Entity(camera)
		{
			_entityType = EntityType::circle;
			_engineBackend.setDrawMode(aux::DrawMode::triangle_fan);
		}

		~Circle() override = default;

		void render() override
		{
			_shader.use();
			_texture.bind();
			_engineBackend.draw();
		}

	private:

	};
}
