#pragma once

#include "seri/core/Entity.h"

namespace seri
{
	class Line : public Entity
	{
	public:
		Line(std::shared_ptr<ICamera> camera) : Entity(camera)
		{
			_entityType = EntityType::line;
			_engineBackend.setDrawMode(aux::DrawMode::line_loop);
		}

		~Line() override = default;

		void Render() override
		{
			_shader.Use();
			_texture.Bind();
			_engineBackend.Draw();
		}

	private:

	};
}
