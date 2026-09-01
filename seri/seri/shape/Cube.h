#pragma once

#include "seri/shape/ShapeEntity.h"

namespace seri
{
	class Cube : public ShapeEntity
	{
	public:
		Cube(std::shared_ptr<CameraBase> camera) : ShapeEntity(camera)
		{
			_entityType = EntityType::cube;
		}

		~Cube() override = default;

		void Render() override
		{
			_shader->Bind();
			_texture->Bind();
		}

	private:

	};
}
