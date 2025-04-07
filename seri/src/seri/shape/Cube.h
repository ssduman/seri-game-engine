#pragma once

#include "seri/core/Entity.h"

namespace seri
{
	class Cube : public Entity
	{
	public:
		Cube(std::shared_ptr<ICamera> camera) : Entity(camera)
		{
			_entityType = EntityType::cube;
			_engineBackend.setDrawMode(aux::DrawMode::triangles);
		}

		~Cube() override = default;

		void render() override
		{
			_shader.use();
			_texture.Bind();
			_engineBackend.draw();
		}

	private:

	};
}
