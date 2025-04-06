#pragma once

#include <GLFW/glfw3.h>

namespace seri
{
	enum class InputAction : int
	{
		press = GLFW_PRESS,
		repeat = GLFW_REPEAT,
		release = GLFW_RELEASE,
	};

	inline const char* toString(InputAction inputAction)
	{
		switch (inputAction)
		{
			case InputAction::press: return "press";
			case InputAction::repeat: return "repeat";
			case InputAction::release: return "release";
			default: return "";
		}
	}
}
