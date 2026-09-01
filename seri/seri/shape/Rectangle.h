#pragma once

#include "seri/shape/ShapeEntity.h"

namespace seri
{
	class Rectangle : public ShapeEntity
	{
	public:
		Rectangle(std::shared_ptr<CameraBase> camera) : ShapeEntity(camera)
		{
			_entityType = EntityType::rectangle;
		}

		~Rectangle() override = default;

		void Render() override
		{
			_shader->Bind();
			_texture->Bind();
		}

	private:

	};
}
