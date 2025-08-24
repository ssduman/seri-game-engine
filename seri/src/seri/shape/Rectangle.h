#pragma once

#include "seri/core/Entity.h"

namespace seri
{
	class Rectangle : public Entity
	{
	public:
		Rectangle(std::shared_ptr<ICamera> camera) : Entity(camera)
		{
			_entityType = EntityType::rectangle;
			_engineBackend.setDrawMode(aux::DrawMode::triangles);
		}

		~Rectangle() override = default;

		void Render() override
		{
			_shader.Use();
			_texture.Bind();
			_engineBackend.Draw();
		}

	private:

	};
}
