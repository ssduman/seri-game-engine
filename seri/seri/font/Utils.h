#pragma once

#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

#include "seri/texture/TextureBase.h"

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

	struct GlyphData
	{
		int unicode;
		int width;
		int height;
		int advance;
		int bearingX;
		int bearingY;
		int uvL;
		int uvB;
		int uvW;
		int uvH;
	};

	struct KerningData
	{
		int unicode_l;
		int unicode_r;
		int x;
		int y;
	};

	struct FontMetadata
	{
		std::string name;
		std::string style;
		std::string renderMode;
		int count;
		int atlasW;
		int atlasH;
		int pointSize;
		int lineHeight;
		int ascentLine;
		int descentLine;
	};

	struct FontInfo
	{
		FontMetadata metadata;
		std::vector<seri::font::GlyphData> glyph_data_list;
		std::vector<seri::font::KerningData> kerning_data_list;
	};

	struct FontData
	{
		FontInfo fontInfo;
		std::shared_ptr<TextureBase> texture;
	};

	std::vector<std::string> split_str(std::string str, std::string delimiter = ",");

	std::string get_pretty_font_name(std::string font_name);

	std::string get_pretty_name(std::string name, std::string style, int pixel_size);

	std::string get_render_mode_str(RenderMode render_mode);

	std::string get_style_mode_str(StyleMode style_mode);

	std::string get_character_set_str(CharacterSet character_set);

	int get_next_power_of_two(int n);

}
