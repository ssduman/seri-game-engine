#pragma once

#include "seri/core/Entity.h"

namespace seri
{
	class Cube : public Entity
	{
	public:
		Cube(std::shared_ptr<CameraBase> camera) : Entity(camera)
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
