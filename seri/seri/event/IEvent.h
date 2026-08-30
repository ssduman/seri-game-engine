#pragma once

#include "seri/event/EventData.h"

namespace seri::event
{
	class IEvent
	{
	public:
		virtual ~IEvent() = default;

		virtual void OnKeyEvent(const KeyEventData& data) {}

		virtual void OnCharacterEvent(const CharacterEventData& data) {}

		virtual void OnCharacterModsEvent(const CharacterModsEventData& data) {}

		virtual void OnMouseEnterEvent(const MouseEnterEventData& data) {}

		virtual void OnMouseButtonEvent(const MouseButtonEventData& data) {}

		virtual void OnMouseScrollEvent(const MouseScrollEventData& data) {}

		virtual void OnMousePositionEvent(const MousePositionEventData& data) {}

		virtual void OnWindowDropEvent(const WindowDropEventData& data) {}

		virtual void OnWindowCloseEvent(const WindowCloseEventData& data) {}

		virtual void OnWindowResizeEvent(const WindowResizeEventData& data) {}

		virtual void OnUserEvent(const IEventData& data) {}

	private:

	};
}
