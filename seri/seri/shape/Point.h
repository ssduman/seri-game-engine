#pragma once

#include "seri/shape/ShapeEntity.h"

namespace seri
{
	class Point : public ShapeEntity
	{
	public:
		Point(std::shared_ptr<CameraBase> camera) : ShapeEntity(camera)
		{
			_entityType = EntityType::point;
		}

		~Point() override = default;

		void Render() override
		{
			_shader->Bind();
			_texture->Bind();
		}

	private:

	};
}
