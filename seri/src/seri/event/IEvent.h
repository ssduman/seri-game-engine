#pragma once

#include "seri/event/EventData.h"

namespace seri::event
{
	class IEvent
	{
	public:
		virtual ~IEvent() = default;

		virtual void onKeyEvent(const KeyEventData& data) {}

		virtual void onCharacterEvent(const CharacterEventData& data) {}

		virtual void onCharacterModsEvent(const CharacterModsEventData& data) {}

		virtual void onMouseEnterEvent(const MouseEnterEventData& data) {}

		virtual void onMouseButtonEvent(const MouseButtonEventData& data) {}

		virtual void onMouseScrollEvent(const MouseScrollEventData& data) {}

		virtual void onMousePositionEvent(const MousePositionEventData& data) {}

		virtual void onWindowDropEvent(const WindowDropEventData& data) {}

		virtual void onWindowCloseEvent(const WindowCloseEventData& data) {}

		virtual void onWindowResizeEvent(const WindowResizeEventData& data) {}

		virtual void onUserEvent(IEventData& data) {}

	private:

	};
}
