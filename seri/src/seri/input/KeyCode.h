#pragma once

#include <GLFW/glfw3.h>

namespace seri
{
	enum class KeyCode : int
	{
		// arrow
		up = GLFW_KEY_UP,
		down = GLFW_KEY_DOWN,
		left = GLFW_KEY_LEFT,
		right = GLFW_KEY_RIGHT,

		// number
		number_0 = GLFW_KEY_0,
		number_1 = GLFW_KEY_1,
		number_2 = GLFW_KEY_2,
		number_3 = GLFW_KEY_3,
		number_4 = GLFW_KEY_4,
		number_5 = GLFW_KEY_5,
		number_6 = GLFW_KEY_6,
		number_7 = GLFW_KEY_7,
		number_8 = GLFW_KEY_8,
		number_9 = GLFW_KEY_9,

		// letter
		a = GLFW_KEY_A,
		b = GLFW_KEY_B,
		c = GLFW_KEY_C,
		d = GLFW_KEY_D,
		e = GLFW_KEY_E,
		f = GLFW_KEY_F,
		g = GLFW_KEY_G,
		h = GLFW_KEY_H,
		i = GLFW_KEY_I,
		j = GLFW_KEY_J,
		k = GLFW_KEY_K,
		l = GLFW_KEY_L,
		m = GLFW_KEY_M,
		n = GLFW_KEY_N,
		o = GLFW_KEY_O,
		p = GLFW_KEY_P,
		q = GLFW_KEY_Q,
		r = GLFW_KEY_R,
		s = GLFW_KEY_S,
		t = GLFW_KEY_T,
		u = GLFW_KEY_U,
		v = GLFW_KEY_V,
		w = GLFW_KEY_W,
		x = GLFW_KEY_X,
		y = GLFW_KEY_Y,
		z = GLFW_KEY_Z,

		// punctuation
		comma = GLFW_KEY_COMMA,
		equal = GLFW_KEY_EQUAL,
		minus = GLFW_KEY_MINUS,
		slash = GLFW_KEY_SLASH,
		period = GLFW_KEY_PERIOD,
		semicolon = GLFW_KEY_SEMICOLON,
		backslash = GLFW_KEY_BACKSLASH,
		apostrophe = GLFW_KEY_APOSTROPHE,
		grave_accent = GLFW_KEY_GRAVE_ACCENT,
		left_bracket = GLFW_KEY_LEFT_BRACKET,
		right_bracket = GLFW_KEY_RIGHT_BRACKET,

		// command
		end = GLFW_KEY_END,
		tab = GLFW_KEY_TAB,
		home = GLFW_KEY_HOME,
		menu = GLFW_KEY_MENU,
		del = GLFW_KEY_DELETE,
		pause = GLFW_KEY_PAUSE,
		enter = GLFW_KEY_ENTER,
		space = GLFW_KEY_SPACE,
		insert = GLFW_KEY_INSERT,
		escape = GLFW_KEY_ESCAPE,
		page_up = GLFW_KEY_PAGE_UP,
		num_lock = GLFW_KEY_NUM_LOCK,
		page_down = GLFW_KEY_PAGE_DOWN,
		caps_lock = GLFW_KEY_CAPS_LOCK,
		backspace = GLFW_KEY_BACKSPACE,
		scroll_lock = GLFW_KEY_SCROLL_LOCK,
		print_screen = GLFW_KEY_PRINT_SCREEN,

		// control
		left_alt = GLFW_KEY_LEFT_ALT,
		left_shift = GLFW_KEY_LEFT_SHIFT,
		left_super = GLFW_KEY_LEFT_SUPER,
		left_control = GLFW_KEY_LEFT_CONTROL,
		right_alt = GLFW_KEY_RIGHT_ALT,
		right_super = GLFW_KEY_RIGHT_SUPER,
		right_shift = GLFW_KEY_RIGHT_SHIFT,
		right_control = GLFW_KEY_RIGHT_CONTROL,

		// f
		f1 = GLFW_KEY_F1,
		f2 = GLFW_KEY_F2,
		f3 = GLFW_KEY_F3,
		f4 = GLFW_KEY_F4,
		f5 = GLFW_KEY_F5,
		f6 = GLFW_KEY_F6,
		f7 = GLFW_KEY_F7,
		f8 = GLFW_KEY_F8,
		f9 = GLFW_KEY_F9,
		f10 = GLFW_KEY_F10,
		f11 = GLFW_KEY_F11,
		f12 = GLFW_KEY_F12,
		f13 = GLFW_KEY_F13,
		f14 = GLFW_KEY_F14,
		f15 = GLFW_KEY_F15,
		f16 = GLFW_KEY_F16,
		f17 = GLFW_KEY_F17,
		f18 = GLFW_KEY_F18,
		f19 = GLFW_KEY_F19,
		f20 = GLFW_KEY_F20,
		f21 = GLFW_KEY_F21,
		f22 = GLFW_KEY_F22,
		f23 = GLFW_KEY_F23,
		f24 = GLFW_KEY_F24,
		f25 = GLFW_KEY_F25,

		// keypad
		kp_0 = GLFW_KEY_KP_0,
		kp_1 = GLFW_KEY_KP_1,
		kp_2 = GLFW_KEY_KP_2,
		kp_3 = GLFW_KEY_KP_3,
		kp_4 = GLFW_KEY_KP_4,
		kp_5 = GLFW_KEY_KP_5,
		kp_6 = GLFW_KEY_KP_6,
		kp_7 = GLFW_KEY_KP_7,
		kp_8 = GLFW_KEY_KP_8,
		kp_9 = GLFW_KEY_KP_9,
		kp_add = GLFW_KEY_KP_ADD,
		kp_enter = GLFW_KEY_KP_ENTER,
		kp_equal = GLFW_KEY_KP_EQUAL,
		kp_divide = GLFW_KEY_KP_DIVIDE,
		kp_decimal = GLFW_KEY_KP_DECIMAL,
		kp_multiply = GLFW_KEY_KP_MULTIPLY,
		kp_subtract = GLFW_KEY_KP_SUBTRACT,

		// other
		last = GLFW_KEY_MENU,
		unknown = GLFW_KEY_UNKNOWN,
		world_1 = GLFW_KEY_WORLD_1,
		world_2 = GLFW_KEY_WORLD_2,
	};

	inline const char* toString(KeyCode keyCode)
	{
		switch (keyCode)
		{
			case KeyCode::up: return "up";
			case KeyCode::down: return "down";
			case KeyCode::left: return "left";
			case KeyCode::right: return "right";
			case KeyCode::number_0: return "number_0";
			case KeyCode::number_1: return "number_1";
			case KeyCode::number_2: return "number_2";
			case KeyCode::number_3: return "number_3";
			case KeyCode::number_4: return "number_4";
			case KeyCode::number_5: return "number_5";
			case KeyCode::number_6: return "number_6";
			case KeyCode::number_7: return "number_7";
			case KeyCode::number_8: return "number_8";
			case KeyCode::number_9: return "number_9";
			case KeyCode::a: return "a";
			case KeyCode::b: return "b";
			case KeyCode::c: return "c";
			case KeyCode::d: return "d";
			case KeyCode::e: return "e";
			case KeyCode::f: return "f";
			case KeyCode::g: return "g";
			case KeyCode::h: return "h";
			case KeyCode::i: return "i";
			case KeyCode::j: return "j";
			case KeyCode::k: return "k";
			case KeyCode::l: return "l";
			case KeyCode::m: return "m";
			case KeyCode::n: return "n";
			case KeyCode::o: return "o";
			case KeyCode::p: return "p";
			case KeyCode::q: return "q";
			case KeyCode::r: return "r";
			case KeyCode::s: return "s";
			case KeyCode::t: return "t";
			case KeyCode::u: return "u";
			case KeyCode::v: return "v";
			case KeyCode::w: return "w";
			case KeyCode::x: return "x";
			case KeyCode::y: return "y";
			case KeyCode::z: return "z";
			case KeyCode::comma: return "comma";
			case KeyCode::equal: return "equal";
			case KeyCode::minus: return "minus";
			case KeyCode::slash: return "slash";
			case KeyCode::period: return "period";
			case KeyCode::semicolon: return "semicolon";
			case KeyCode::backslash: return "backslash";
			case KeyCode::apostrophe: return "apostrophe";
			case KeyCode::grave_accent: return "grave_accent";
			case KeyCode::left_bracket: return "left_bracket";
			case KeyCode::right_bracket: return "right_bracket";
			case KeyCode::end: return "end";
			case KeyCode::tab: return "tab";
			case KeyCode::home: return "home";
			case KeyCode::menu: return "menu";
			case KeyCode::del: return "del";
			case KeyCode::pause: return "pause";
			case KeyCode::enter: return "enter";
			case KeyCode::space: return "space";
			case KeyCode::insert: return "insert";
			case KeyCode::escape: return "escape";
			case KeyCode::page_up: return "page_up";
			case KeyCode::num_lock: return "num_lock";
			case KeyCode::page_down: return "page_down";
			case KeyCode::caps_lock: return "caps_lock";
			case KeyCode::backspace: return "backspace";
			case KeyCode::scroll_lock: return "scroll_lock";
			case KeyCode::print_screen: return "print_screen";
			case KeyCode::left_alt: return "left_alt";
			case KeyCode::left_shift: return "left_shift";
			case KeyCode::left_super: return "left_super";
			case KeyCode::left_control: return "left_control";
			case KeyCode::right_alt: return "right_alt";
			case KeyCode::right_super: return "right_super";
			case KeyCode::right_shift: return "right_shift";
			case KeyCode::right_control: return "right_control";
			case KeyCode::f1: return "f1";
			case KeyCode::f2: return "f2";
			case KeyCode::f3: return "f3";
			case KeyCode::f4: return "f4";
			case KeyCode::f5: return "f5";
			case KeyCode::f6: return "f6";
			case KeyCode::f7: return "f7";
			case KeyCode::f8: return "f8";
			case KeyCode::f9: return "f9";
			case KeyCode::f10: return "f10";
			case KeyCode::f11: return "f11";
			case KeyCode::f12: return "f12";
			case KeyCode::f13: return "f13";
			case KeyCode::f14: return "f14";
			case KeyCode::f15: return "f15";
			case KeyCode::f16: return "f16";
			case KeyCode::f17: return "f17";
			case KeyCode::f18: return "f18";
			case KeyCode::f19: return "f19";
			case KeyCode::f20: return "f20";
			case KeyCode::f21: return "f21";
			case KeyCode::f22: return "f22";
			case KeyCode::f23: return "f23";
			case KeyCode::f24: return "f24";
			case KeyCode::f25: return "f25";
			case KeyCode::kp_0: return "kp_0";
			case KeyCode::kp_1: return "kp_1";
			case KeyCode::kp_2: return "kp_2";
			case KeyCode::kp_3: return "kp_3";
			case KeyCode::kp_4: return "kp_4";
			case KeyCode::kp_5: return "kp_5";
			case KeyCode::kp_6: return "kp_6";
			case KeyCode::kp_7: return "kp_7";
			case KeyCode::kp_8: return "kp_8";
			case KeyCode::kp_9: return "kp_9";
			case KeyCode::kp_add: return "kp_add";
			case KeyCode::kp_enter: return "kp_enter";
			case KeyCode::kp_equal: return "kp_equal";
			case KeyCode::kp_divide: return "kp_divide";
			case KeyCode::kp_decimal: return "kp_decimal";
			case KeyCode::kp_multiply: return "kp_multiply";
			case KeyCode::kp_subtract: return "kp_subtract";
			case KeyCode::unknown: return "unknown";
			case KeyCode::world_1: return "world_1";
			case KeyCode::world_2: return "world_2";
			default: return "";
		}
	}
}
