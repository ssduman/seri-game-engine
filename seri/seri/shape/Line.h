#pragma once

#include "seri/core/Entity.h"

namespace seri
{
	class Line : public Entity
	{
	public:
		Line(std::shared_ptr<CameraBase> camera) : Entity(camera)
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
