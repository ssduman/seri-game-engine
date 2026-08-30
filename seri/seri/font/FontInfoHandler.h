#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <filesystem>

#include <nlohmann/json.hpp>

#include "seri/font/Utils.h"

namespace seri::font
{
	struct FontInfoHandlerParams
	{
		std::string font_name;
		std::string output_folder_path;
	};

	class FontInfoHandler
	{
	public:
		FontInfoHandler(FontInfoHandlerParams params);
		~FontInfoHandler() = default;

		void Reset();
		void PutFontMetada(
			std::string name, std::string style, std::string render_mode_str,
			int count, int texture_width, int texture_height, int pixel_size,
			int height, int ascender, int descender
		);
		void AddGlyphData(int unicode);
		void AddGlyphData(int unicode, int width, int height, int advance, int bearingX, int bearingY, int uvL, int uvB, int uvW, int uvH);
		void AddKerningData(int unicode_l, int unicode_r, int x, int y);

		FontInfo GetFontInfo();
		FontInfo CloneFontInfo();

		std::string Dump(int indent = 2);
		void Write(std::string name, std::string style, int calculated_height, int pixel_size, int indent = 2);

	private:
		FontInfoHandlerParams _params;

		const std::string output_folder_name = "infos";
		std::filesystem::path output_path;

		FontInfo _font_info{};

	};
}
