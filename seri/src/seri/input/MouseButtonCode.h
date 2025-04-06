#pragma once

#include <GLFW/glfw3.h>

namespace seri
{
	enum class MouseButtonCode : int
	{
		button_1 = GLFW_MOUSE_BUTTON_1,
		button_2 = GLFW_MOUSE_BUTTON_2,
		button_3 = GLFW_MOUSE_BUTTON_3,
		button_4 = GLFW_MOUSE_BUTTON_4,
		button_5 = GLFW_MOUSE_BUTTON_5,
		button_6 = GLFW_MOUSE_BUTTON_6,
		button_7 = GLFW_MOUSE_BUTTON_7,
		button_8 = GLFW_MOUSE_BUTTON_8,
		button_last = GLFW_MOUSE_BUTTON_LAST,
		button_left = GLFW_MOUSE_BUTTON_LEFT,
		button_right = GLFW_MOUSE_BUTTON_RIGHT,
		button_middle = GLFW_MOUSE_BUTTON_MIDDLE,
	};

	inline const char* toString(MouseButtonCode mouseButtonCode)
	{
		switch (mouseButtonCode)
		{
			case MouseButtonCode::button_4: return "button_4";
			case MouseButtonCode::button_5: return "button_5";
			case MouseButtonCode::button_6: return "button_6";
			case MouseButtonCode::button_7: return "button_7";
			case MouseButtonCode::button_last: return "button_last";
			case MouseButtonCode::button_left: return "button_left";
			case MouseButtonCode::button_right: return "button_right";
			case MouseButtonCode::button_middle: return "button_middle";
			default: return "";
		}
	}
}
