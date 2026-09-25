#pragma once

namespace seri::util
{
	enum class BackgroundMode
	{
		skybox,
		color,
	};

	inline const char* BackgroundModeToString(BackgroundMode mode)
	{
		switch (mode)
		{
			case BackgroundMode::skybox: return "skybox";
			case BackgroundMode::color: return "color";
			default: return "unknown";
		};
	}
}
