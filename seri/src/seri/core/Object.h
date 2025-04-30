#pragma once

#include "seri/util/Util.h"
#include "seri/event/IEvent.h"

namespace seri
{
	class Object : public event::IEvent
	{
	public:
		virtual ~Object() = default;

		virtual void init() = 0;

		virtual void update() = 0;

		virtual void render() = 0;

		virtual void display()
		{
			update();
			render();
		}

		virtual glm::vec3 GetPosition()
		{
			return glm::vec3{ 0.0f, 0.0f, 0.0f };
		}

		virtual void SetPosition(glm::vec3 p)
		{
		}

	};
}
