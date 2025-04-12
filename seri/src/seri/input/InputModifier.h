#pragma once

#include "seri/renderer/AuxiliaryStructs.h"

namespace seri
{
	enum class InputModifier : int
	{
		noop = 0,
		alt = GLFW_MOD_ALT,
		shift = GLFW_MOD_SHIFT,
		super = GLFW_MOD_SUPER,
		control = GLFW_MOD_CONTROL,
		num_lock = GLFW_MOD_NUM_LOCK,
		caps_lock = GLFW_MOD_CAPS_LOCK,
	};

	inline const char* toString(InputModifier inputModifier)
	{
		switch (inputModifier)
		{
			case InputModifier::noop: return "noop";
			case InputModifier::alt: return "alt";
			case InputModifier::shift: return "shift";
			case InputModifier::super: return "super";
			case InputModifier::control: return "control";
			case InputModifier::num_lock: return "num_lock";
			case InputModifier::caps_lock: return "caps_lock";
			default: return "";
		}
	}
}
