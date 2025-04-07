#pragma once

#pragma warning(push)
#pragma warning(disable: 4018)
#pragma warning(disable: 4099)
#pragma warning(disable: 4244)
#pragma warning(disable: 4267)

#include <cmath>
#include <regex>
#include <chrono>
#include <vector>
#include <fstream>
#include <iostream>

#include <ft2build.h>
#include <freetype/ftbbox.h>
#include <freetype/ftsynth.h>
#include <freetype/ftadvanc.h>
#include <freetype/ftbitmap.h>
#include <freetype/ftdriver.h>
#include <freetype/fterrors.h>
#include <freetype/ftfntfmt.h>
#include <freetype/ftmodapi.h>
#include <freetype/ftstroke.h>
#include FT_FREETYPE_H

#include "Utils.h"
#include "GlyphPacker.h"
#include "ImageWriter.h"
#include "FontInfoHandler.h"

namespace seri::font
{
	struct FontGeneratorParams
	{
		FontGeneratorParams()
		{
			SetDefault();
		}

		std::string input_folder_path;
		std::string output_folder_path;
		std::string font_name;
		int texture_width;
		int texture_height;
		CharacterSet character_set;
		bool get_kerning;
		RenderMode render_mode;

	private:
		void SetDefault()
		{
			this->input_folder_path = "assets/fonts";
			this->output_folder_path = "assets/fonts";
			this->font_name = "Roboto-Regular";
			this->texture_width = 1024;
			this->texture_height = 1024;
			this->character_set = CharacterSet::charset_extended_ascii;
			this->get_kerning = false;
			this->render_mode = seri::font::RenderMode::render_light;
		}
	};

	struct FontGeneratorInitParams
	{
		FontGeneratorInitParams()
		{
			SetDefault();
		}

		int pixel_size;
		StyleMode style_mode;

	private:
		void SetDefault()
		{
			this->pixel_size = 48;
			this->style_mode = StyleMode::style_default;
		}
	};

	class FontGenerator
	{
	public:
		FontGenerator(FontGeneratorParams params);
		~FontGenerator();

		void Init(FontGeneratorInitParams init_params);

		void Generate();

	private:
		FontGeneratorParams _params;
		FontGeneratorInitParams _init_params{};

		FT_Error _error = 0;
		FT_Library _library = nullptr;
		FT_Face _face = nullptr;

		FT_Int32 _load_flags = FT_LOAD_DEFAULT;
		FT_Render_Mode _render_mode = FT_RENDER_MODE_NORMAL;

		bool _has_hinter = false;
		bool _has_kerning = false;
		bool _is_fixed_width = false;
		bool _is_bold = false;
		bool _is_italic = false;

		std::string _ft_version;
		std::string _module_name;
		std::string _hinting_engine;

		std::vector<int> _unicode_ranges;

		GlyphPacker* _glyph_packer = nullptr;
		ImageWriter* _image_writer = nullptr;
		FontInfoHandler* _font_info_handler = nullptr;

		void InitFreeType();
		void InitUtilClass();

		void GetAndPrintFontInfo();

		void MakeASCII();
		void MakeExtendedASCII();
		void MakeCyrillic();

		void GenerateGlyph(int unicode, FT_Int32 load_flags, FT_Render_Mode render_mode);
		void GetKerning(int unicode);

		void FillUnicodeRange(int beg);
		void FillUnicodeRangesInclusive(int beg, int end);

		FT_Vector MakeVector(float delta_x = 0.0f, float delta_y = 0.0f);
		FT_Matrix MakeMatrix(float scale = 1.0f, float skew_in_rad = 0.0f);

	};
}
