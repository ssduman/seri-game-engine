#pragma once

#include "seri/shape/ShapeEntity.h"

namespace seri
{
	class Triangle : public ShapeEntity
	{
	public:
		Triangle(std::shared_ptr<CameraBase> camera) : ShapeEntity(camera)
		{
			_entityType = EntityType::triangle;
		}

		~Triangle() override = default;

		void Render() override
		{
			_shader->Bind();
			_texture->Bind();
		}

	private:

	};
}
