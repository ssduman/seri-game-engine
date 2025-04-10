#pragma once

#include "seri/core/Entity.h"

namespace seri
{
	class Point : public Entity
	{
	public:
		Point(std::shared_ptr<ICamera> camera) : Entity(camera)
		{
			_entityType = EntityType::point;
			_engineBackend.setDrawMode(aux::DrawMode::points);
		}

		~Point() override = default;

		void render() override
		{
			_shader.use();
			_texture.Bind();
			_engineBackend.draw();
		}

	private:

	};
}
