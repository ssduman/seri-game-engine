#pragma once

#include "seri/shape/ShapeEntity.h"

namespace seri
{
	class Line : public ShapeEntity
	{
	public:
		Line(std::shared_ptr<CameraBase> camera) : ShapeEntity(camera)
		{
			_entityType = EntityType::line;
		}

		~Line() override = default;

		void Render() override
		{
			_shader->Bind();
			_texture->Bind();
		}

	private:

	};
}
