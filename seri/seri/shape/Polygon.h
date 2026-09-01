#pragma once

#include "seri/shape/ShapeEntity.h"

namespace seri
{
	class Polygon : public ShapeEntity
	{
	public:
		Polygon(std::shared_ptr<CameraBase> camera) : ShapeEntity(camera)
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
