#pragma once

#include "seri/core/Entity.h"

namespace seri
{
	class Polygon : public Entity
	{
	public:
		Polygon(std::shared_ptr<CameraBase> camera) : Entity(camera)
		{
			_entityType = EntityType::polygon;
		}

		~Polygon() override = default;

		void Render() override
		{
			_shader->Bind();
			_texture->Bind();
		}

	private:

	};
}
