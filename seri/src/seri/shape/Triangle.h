#pragma once

#include "seri/core/Entity.h"

namespace seri
{
	class Triangle : public Entity
	{
	public:
		Triangle(std::shared_ptr<ICamera> camera) : Entity(camera)
		{
			_entityType = EntityType::triangle;
			_engineBackend.setDrawMode(aux::DrawMode::triangles);
		}

		~Triangle() override = default;

		void Render() override
		{
			_shader.Use();
			_texture.Bind();
			_engineBackend.Draw();
		}

	private:

	};
}
