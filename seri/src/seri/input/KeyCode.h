#pragma once

namespace seri
{
	enum class InputAction : int
	{
		noop = -1,

		press,
		repeat,
		release,

		len
	};

	enum class InputModifier : int
	{
		noop = -1,

		alt,
		shift,
		super,
		control,
		num_lock,
		caps_lock,

		len
	};

	enum class MouseButtonCode : int
	{
		noop = -1,

		button_left,
		button_right,
		button_middle,
		button_4,
		button_5,
		button_6,
		button_7,
		button_8,

		len
	};

	enum class KeyCode : int
	{
		// arrow
		up,
		down,
		left,
		right,

		// number
		number_0,
		number_1,
		number_2,
		number_3,
		number_4,
		number_5,
		number_6,
		number_7,
		number_8,
		number_9,

		// letter
		a,
		b,
		c,
		d,
		e,
		f,
		g,
		h,
		i,
		j,
		k,
		l,
		m,
		n,
		o,
		p,
		q,
		r,
		s,
		t,
		u,
		v,
		w,
		x,
		y,
		z,

		// punctuation
		comma,
		equal,
		minus,
		slash,
		period,
		semicolon,
		backslash,
		apostrophe,
		grave_accent,
		left_bracket,
		right_bracket,

		// command
		end,
		tab,
		home,
		menu,
		del,
		pause,
		enter,
		space,
		insert,
		escape,
		page_up,
		num_lock,
		page_down,
		caps_lock,
		backspace,
		scroll_lock,
		print_screen,

		// control
		left_alt,
		left_shift,
		left_super,
		left_control,
		right_alt,
		right_shift,
		right_super,
		right_control,

		// f
		f1,
		f2,
		f3,
		f4,
		f5,
		f6,
		f7,
		f8,
		f9,
		f10,
		f11,
		f12,
		f13,
		f14,
		f15,
		f16,
		f17,
		f18,
		f19,
		f20,
		f21,
		f22,
		f23,
		f24,

		// keypad
		kp_0,
		kp_1,
		kp_2,
		kp_3,
		kp_4,
		kp_5,
		kp_6,
		kp_7,
		kp_8,
		kp_9,
		kp_add,
		kp_enter,
		kp_equal,
		kp_divide,
		kp_decimal,
		kp_multiply,
		kp_subtract,

		// other
		last,
		unknown,

		len,
	};

	inline const char* ToString(InputAction inputAction)
	{
		switch (inputAction)
		{
			case InputAction::noop: return "noop";
			case InputAction::press: return "press";
			case InputAction::repeat: return "repeat";
			case InputAction::release: return "release";
			default: return "";
		}
	}

	inline const char* ToString(InputModifier inputModifier)
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

	inline const char* ToString(MouseButtonCode mouseButtonCode)
	{
		switch (mouseButtonCode)
		{
			case MouseButtonCode::noop: return "noop";
			case MouseButtonCode::button_left: return "button_left";
			case MouseButtonCode::button_right: return "button_right";
			case MouseButtonCode::button_middle: return "button_middle";
			case MouseButtonCode::button_4: return "button_4";
			case MouseButtonCode::button_5: return "button_5";
			case MouseButtonCode::button_6: return "button_6";
			case MouseButtonCode::button_7: return "button_7";
			case MouseButtonCode::button_8: return "button_8";
			default: return "";
		}
	}

	inline const char* ToString(KeyCode keyCode)
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
			default: return "";
		}
	}

}
