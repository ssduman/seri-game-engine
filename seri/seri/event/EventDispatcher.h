#pragma once

#include "seri/event/EventData.h"

#include <stdexcept>
#include <functional>

namespace seri::event
{
	struct EventDispatcher
	{
		void operator()(const IEventData& data)
		{
			//LOGGER(info, "event: " << data.ToString());

			switch (data.eventType)
			{
				case EventType::key:
					{
						auto& d = CastEventData<KeyEventData&>(data);
					}
					break;
				case EventType::character:
					{
						auto d = CastEventData<CharacterEventData&>(data);
					}
					break;
				case EventType::character_mods:
					{
						auto& d = CastEventData<CharacterModsEventData&>(data);
					}
					break;
				case EventType::mouse_enter:
					{
						auto& d = CastEventData<MouseEnterEventData&>(data);
					}
					break;
				case EventType::mouse_button:
					{
						auto& d = CastEventData<MouseButtonEventData&>(data);
					}
					break;
				case EventType::mouse_scroll:
					{
						auto& d = CastEventData<MouseScrollEventData&>(data);
					}
					break;
				case EventType::mouse_position:
					{
						auto& d = CastEventData<MousePositionEventData&>(data);
					}
					break;
				case EventType::window_drop:
					{
						auto& d = CastEventData<WindowDropEventData&>(data);
					}
					break;
				case EventType::window_close:
					{
						auto& d = CastEventData<WindowCloseEventData&>(data);
					}
					break;
				case EventType::window_resize:
					{
						auto& d = CastEventData<WindowResizeEventData&>(data);
					}
					break;
				case EventType::tick:
					{
					}
					break;
				case EventType::user:
					{
					}
					break;
				case EventType::unknown:
					break;
				default:
					break;
			}
		}

		template<typename T>
		static T& CastEventData(const IEventData& data)
		{
			return dynamic_cast<T&>(const_cast<IEventData&>(data));
		}

	private:

	};
}
