#pragma once

namespace seri::ui
{
	enum class CanvasRenderMode
	{
		screen_space = 0,
		world_space = 1,
	};

	inline const char* CanvasRenderModeToString(CanvasRenderMode mode)
	{
		switch (mode)
		{
			case CanvasRenderMode::screen_space: return "screen space";
			case CanvasRenderMode::world_space: return "world space";
			default: return "unknown";
		}
	}
}
