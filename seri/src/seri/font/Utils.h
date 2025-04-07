#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

namespace seri::font
{
	enum DrawMode
	{
		draw_main,

		draw_len,
	};

	enum RenderMode
	{
		render_normal,
		render_light,
		render_force_autohint,
		render_no_autohint,
		render_no_hint,

		render_len,
	};

	enum StyleMode
	{
		style_default,
		style_regular,
		style_italic,
		style_bold,

		style_len,
	};

	enum CharacterSet
	{
		charset_ascii,
		charset_extended_ascii,
		charset_cyrillic,

		charset_len
	};

	const static double DEGREE_TO_RADIAN = 3.1415926 / 180.0;
	const static double RADIAN_TO_DEGREE = 180.0 / 3.1415926;

	std::vector<std::string> split_str(std::string str, std::string delimiter = ",");

	std::string get_pretty_font_name(std::string font_name);

	std::string get_pretty_name(std::string name, std::string style, int pixel_size);

	std::string get_render_mode_str(RenderMode render_mode);

	std::string get_style_mode_str(StyleMode style_mode);

	std::string get_character_set_str(CharacterSet character_set);

	int get_next_power_of_two(int n);

}
