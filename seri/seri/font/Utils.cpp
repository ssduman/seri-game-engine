#include "Seripch.h"

#include "seri/logging/Logger.h"

#include "Utils.h"

namespace seri::font
{

	std::vector<std::string> split_str(std::string str, std::string delimiter)
	{
		try
		{
			size_t pos = 0;
			std::string token{};
			std::string str_copy = std::string{ str };
			std::vector<std::string> ret{};
			while ((pos = str_copy.find(delimiter)) != std::string::npos)
			{
				token = str_copy.substr(0, pos);
				ret.push_back(token);
				str_copy.erase(0, pos + delimiter.length());
			}
			ret.push_back(str_copy);
			return ret;
		}
		catch (const std::exception& e)
		{
			LOGGER(error, "[font] exception occurred while splitting: " << str << ", exception: " << e.what());
			return {};
		}
	}

	std::string get_pretty_font_name(std::string font_name)
	{
		std::string pretty_name = font_name;
		std::transform(pretty_name.begin(), pretty_name.end(), pretty_name.begin(), [](unsigned char c) { return std::tolower(c); });
		return pretty_name;
	}

	std::string get_pretty_name(std::string name, std::string style, int pixel_size)
	{
		std::string pretty_name = name + "_" + style + "_" + std::to_string(pixel_size);
		std::transform(pretty_name.begin(), pretty_name.end(), pretty_name.begin(), [](unsigned char c) { return std::tolower(c); });
		std::replace(pretty_name.begin(), pretty_name.end(), ' ', '_');
		std::replace(pretty_name.begin(), pretty_name.end(), '-', '_');
		return pretty_name;
	}

	std::string get_render_mode_str(RenderMode render_mode)
	{
		switch (render_mode)
		{
			case RenderMode::render_normal:
				return "normal";
			case RenderMode::render_light:
				return "light";
			case RenderMode::render_force_autohint:
				return "force_autohint";
			case RenderMode::render_no_autohint:
				return "no_autohint";
			case RenderMode::render_no_hint:
				return "no_hint";
			default:
				return "unknown";
		}
	}

	std::string get_style_mode_str(StyleMode style_mode)
	{
		switch (style_mode)
		{
			case seri::font::style_default:
				return "default";
			case seri::font::style_regular:
				return "regular";
			case seri::font::style_italic:
				return "italic";
			case seri::font::style_bold:
				return "bold";
			default:
				return "unknown";
		}
	}

	std::string get_character_set_str(CharacterSet character_set)
	{
		switch (character_set)
		{
			case seri::font::charset_ascii:
				return "ascii";
			case seri::font::charset_extended_ascii:
				return "extended_ascii";
			case seri::font::charset_cyrillic:
				return "cyrillic";
			default:
				return "unknown";
		}
	}

	int get_next_power_of_two(int x)
	{
		if (x < 0)
		{
			return 0;
		}

		--x;
		x |= x >> 1;
		x |= x >> 2;
		x |= x >> 4;
		x |= x >> 8;
		x |= x >> 16;
		return x + 1;
	}

}
