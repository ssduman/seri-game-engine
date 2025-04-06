#pragma once

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

	};
}
