#pragma once

#include "seri/core/Entity.h"

namespace seri
{
	class Polygon : public Entity
	{
	public:
		Polygon(std::shared_ptr<ICamera> camera) : Entity(camera)
		{
			_entityType = EntityType::polygon;
			_engineBackend.setDrawMode(aux::DrawMode::triangle_fan);
		}

		~Polygon() override = default;

		void render() override
		{
			_shader.use();
			_texture.Bind();
			_engineBackend.draw();
		}

	private:

	};
}
