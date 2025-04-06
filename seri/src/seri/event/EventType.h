#pragma once

namespace seri::event
{
	enum class EventType
	{
		key,
		character,
		character_mods,
		mouse_enter,
		mouse_button,
		mouse_scroll,
		mouse_position,
		window_drop,
		window_close,
		window_resize,
		tick,
		user,
		unknown,
	};

	inline const char* toString(EventType eventType)
	{
		switch (eventType)
		{
			case EventType::key: return "key";
			case EventType::character: return "character";
			case EventType::character_mods: return "character_mods";
			case EventType::mouse_enter: return "mouse_enter";
			case EventType::mouse_button: return "mouse_button";
			case EventType::mouse_scroll: return "mouse_scroll";
			case EventType::mouse_position: return "mouse_position";
			case EventType::window_drop: return "window_drop";
			case EventType::window_close: return "window_close";
			case EventType::window_resize: return "window_resize";
			case EventType::tick: return "tick";
			case EventType::user: return "user";
			case EventType::unknown: return "unknown";
			default: return "";
		}
	}
}
