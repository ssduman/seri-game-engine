#pragma once

#include "seri/util/Util.h"
#include "seri/event/IEvent.h"

namespace seri
{
	class Object : public event::IEvent
	{
	public:
		virtual ~Object() = default;

		virtual void Init() = 0;

		virtual void Update() = 0;

		virtual void Render() = 0;

		virtual void Display()
		{
			Update();
			Render();
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
