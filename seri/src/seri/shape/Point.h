#pragma once

#include "seri/core/Entity.h"

namespace seri
{
	class Point : public Entity
	{
	public:
		Point(std::shared_ptr<CameraBase> camera) : Entity(camera)
		{
			_entityType = EntityType::point;
			_engineBackend.setDrawMode(aux::DrawMode::points);
		}

		~Point() override = default;

		void Render() override
		{
			_shader->Bind();
			_texture->Bind();
			_engineBackend.Draw();
		}

	private:

	};
}
