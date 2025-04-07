#include "Seripch.h"

#include "seri/logging/Logger.h"

#include "FontGenerator.h"

namespace seri::font
{
	FontGenerator::FontGenerator(FontGeneratorParams params)
		: _params{ params }
	{
		// get character set
		switch (_params.character_set)
		{
			case CharacterSet::charset_ascii:
				MakeASCII();
				break;
			case CharacterSet::charset_extended_ascii:
				MakeExtendedASCII();
				break;
			case CharacterSet::charset_cyrillic:
				MakeCyrillic();
				break;
			default:
				LOGGER(error, "[font] unknown character set: " << get_character_set_str(_params.character_set));
				return;
		}

		// init
		InitFreeType();
		InitUtilClass();

		// get load flags
		switch (_params.render_mode)
		{
			case RenderMode::render_normal:
				_load_flags = FT_LOAD_TARGET_NORMAL;
				break;
			case RenderMode::render_light:
				_load_flags = FT_LOAD_TARGET_LIGHT;
				break;
			case RenderMode::render_force_autohint:
				_load_flags = FT_LOAD_DEFAULT | FT_LOAD_FORCE_AUTOHINT;
				break;
			case RenderMode::render_no_autohint:
				_load_flags = FT_LOAD_DEFAULT | FT_LOAD_NO_AUTOHINT;
				break;
			case RenderMode::render_no_hint:
				_load_flags = FT_LOAD_DEFAULT | FT_LOAD_NO_HINTING;
				break;
		}

		// get render mode
		switch (_params.render_mode)
		{
			case RenderMode::render_normal:
				_render_mode = FT_RENDER_MODE_NORMAL;
				break;
			case RenderMode::render_light:
				_render_mode = FT_RENDER_MODE_LIGHT;
				break;
			case RenderMode::render_force_autohint:
				_render_mode = FT_RENDER_MODE_NORMAL;
				break;
			case RenderMode::render_no_autohint:
				_render_mode = FT_RENDER_MODE_NORMAL;
				break;
		}

		// log
		GetAndPrintFontInfo();
	}

	FontGenerator::~FontGenerator()
	{
		free(_glyph_packer);
		free(_image_writer);
		free(_font_info_handler);

		FT_Done_Face(_face);
		FT_Done_FreeType(_library);
	}

	void FontGenerator::InitFreeType()
	{
		_error = FT_Init_FreeType(&_library);
		if (_error)
		{
			LOGGER(error, "[font] init ft failed: " << FT_Error_String(_error));
			return;
		}

		_error = FT_New_Face(_library, (_params.input_folder_path + "/" + _params.font_name + ".ttf").c_str(), /*face_index*/ 0, &_face);
		if (_error)
		{
			LOGGER(error, "[font] load font face failed: " << FT_Error_String(_error));
			return;
		}

		_error = FT_Select_Charmap(_face, FT_ENCODING_UNICODE);
		if (_error)
		{
			LOGGER(error, "[font] setting char map failed: " << FT_Error_String(_error));
			return;
		}
	}

	void FontGenerator::InitUtilClass()
	{
		GlyphPackerParams glyph_packer_params{};
		glyph_packer_params.output_folder_path = _params.output_folder_path;
		glyph_packer_params.texture_width = _params.texture_width;
		glyph_packer_params.texture_height = _params.texture_height;
		glyph_packer_params.num_rect = _unicode_ranges.size();
		glyph_packer_params.padding = 4;
		_glyph_packer = new GlyphPacker(glyph_packer_params);

		ImageWriterParams image_writer_params{};
		image_writer_params.font_name = _params.font_name;
		image_writer_params.output_folder_path = _params.output_folder_path;
		image_writer_params.texture_width = _params.texture_width;
		image_writer_params.texture_height = _params.texture_height;
		image_writer_params.texture_comp = 4;
		_image_writer = new ImageWriter(image_writer_params);

		FontInfoHandlerParams font_info_handler_params{};
		font_info_handler_params.font_name = _params.font_name;
		font_info_handler_params.output_folder_path = _params.output_folder_path;
		_font_info_handler = new FontInfoHandler(font_info_handler_params);
	}

	void FontGenerator::GetAndPrintFontInfo()
	{
		// version
		FT_Int major, minor, patch;
		FT_Library_Version(_library, &major, &minor, &patch);
		_ft_version = std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch);

		// face info
		FT_Long face_flags = _face->face_flags;
		_has_hinter = (face_flags & FT_FACE_FLAG_HINTER) != 0;
		_has_kerning = (face_flags & FT_FACE_FLAG_KERNING) != 0;
		_is_fixed_width = (face_flags & FT_FACE_FLAG_FIXED_WIDTH) != 0;

		// style info
		FT_Long style_flags = _face->style_flags;
		_is_bold = (style_flags & FT_STYLE_FLAG_BOLD) != 0;
		_is_italic = (style_flags & FT_STYLE_FLAG_ITALIC) != 0;

		// module and hinting engine info
		_module_name = FT_FACE_DRIVER_NAME(_face);
		FT_UInt prop;
		if (!FT_IS_SCALABLE(_face))
		{
			_hinting_engine = "bitmap";
		}
		else if (_load_flags & FT_LOAD_NO_SCALE)
		{
			_hinting_engine = "unscaled";
		}
		else if (_load_flags & FT_LOAD_NO_HINTING)
		{
			_hinting_engine = "unhinted";
		}
		else if (_render_mode == FT_RENDER_MODE_LIGHT)
		{
			_hinting_engine = "auto";
		}
		else if (_render_mode == FT_LOAD_FORCE_AUTOHINT)
		{
			_hinting_engine = "auto";
		}
		else if (!FT_Property_Get(_library, _module_name.c_str(), "interpreter-version", &prop))
		{
			switch (prop)
			{
				case TT_INTERPRETER_VERSION_35:
					_hinting_engine = "v35";
					break;
				case TT_INTERPRETER_VERSION_40:
					_hinting_engine = "v40";
					break;
			}
		}
		else if (!FT_Property_Get(_library, _module_name.c_str(), "hinting-engine", &prop))
		{
			switch (prop)
			{
				case FT_HINTING_FREETYPE:
					_hinting_engine = "FT";
					break;
				case FT_HINTING_ADOBE:
					_hinting_engine = "Adobe";
					break;
			}
		}

		static bool write_log = false;
		if (write_log)
		{
			LOGGER(info,
				"[font] font info:" << "\n" <<
				" freetype version: " << _ft_version << "\n" <<
				" face has hinter: " << (_has_hinter ? "true" : "false") << "\n" <<
				" face has kerning: " << (_has_kerning ? "true" : "false") << "\n" <<
				" face is fixed width: " << (_is_fixed_width ? "true" : "false") << "\n" <<
				" style is bold: " << (_is_bold ? "true" : "false") << "\n" <<
				" style is italic: " << (_is_italic ? "true" : "false") << "\n" <<
				" module name: " << _module_name << "\n" <<
				" hinting engine: " << _hinting_engine
			);
		}
	}

	void FontGenerator::MakeASCII()
	{
		_unicode_ranges.clear();
		FillUnicodeRange(3);
		FillUnicodeRange(9);
		FillUnicodeRange(10);
		FillUnicodeRange(11);
		FillUnicodeRange(13);
		FillUnicodeRangesInclusive(32, 126);
		FillUnicodeRangesInclusive(160, 255);
	}

	void FontGenerator::MakeExtendedASCII()
	{
		MakeASCII();
		FillUnicodeRangesInclusive(286, 287);
		FillUnicodeRangesInclusive(304, 305);
		FillUnicodeRangesInclusive(350, 351);
	}

	void FontGenerator::MakeCyrillic()
	{
		MakeExtendedASCII();
		FillUnicodeRangesInclusive(1024, 1279);
	}

	void FontGenerator::Init(FontGeneratorInitParams init_params)
	{
		// set init params
		_init_params = init_params;

		// set pixel/char size
#ifdef SERI_FONT_USE_PIXEL_SIZE
		_error = FT_Set_Pixel_Sizes(_face, /*pixel_width*/ 0, /*pixel_height*/ _init_params.pixel_size);
		if (_error)
		{
			LOGGER(error, "[font] setting pixel size failed: " << FT_Error_String(_error));
			return;
		}
#else
		_error = FT_Set_Char_Size(_face, /*char_width*/ 0, /*char_height*/ _init_params.pixel_size * 64, /*horz_resolution*/ 0, /*vert_resolution*/ 0);
		if (_error)
		{
			LOGGER(error, "[font] setting char size failed: " << FT_Error_String(_error));
			return;
		}
#endif

		// font name
		std::string fontName = std::string{ _face->family_name };
		std::replace(fontName.begin(), fontName.end(), ' ', '_');
		std::transform(fontName.begin(), fontName.end(), fontName.begin(), [](unsigned char c) { return std::tolower(c); });

		// font style name
		std::string fontStyleName = _init_params.style_mode == StyleMode::style_default ? _face->style_name : get_style_mode_str(_init_params.style_mode);
		std::replace(fontStyleName.begin(), fontStyleName.end(), ' ', '_');
		std::transform(fontStyleName.begin(), fontStyleName.end(), fontStyleName.begin(), [](unsigned char c) { return std::tolower(c); });

		// put font metadata
		_font_info_handler->PutFontMetada(
			fontName,
			fontStyleName,
			get_render_mode_str(_params.render_mode),
			_unicode_ranges.size(),
			_params.texture_width,
			_params.texture_height,
			_init_params.pixel_size,
			std::round(_face->size->metrics.height / 64.0),
			std::round(_face->size->metrics.ascender / 64.0),
			std::round(_face->size->metrics.descender / 64.0)
		);

		// log
		FontMetadata font_metadata = _font_info_handler->GetFontInfo().metadata;
		LOGGER(info, "[font] inited font: " << font_metadata.name << ", style: " << font_metadata.style << ", size: " << font_metadata.pointSize);
	}

	std::shared_ptr<FontData> FontGenerator::Generate()
	{
		// chronometer beg
		std::chrono::steady_clock::time_point beg = std::chrono::steady_clock::now();

		// generate glyphs
		for (int index = 0; index < _unicode_ranges.size(); index++)
		{
			int unicode = _unicode_ranges.at(index);
			GenerateGlyph(unicode, _load_flags, _render_mode);
			if (_params.get_kerning && _has_kerning)
			{
				GetKerning(unicode);
			}
		}

		// write data
		FontMetadata font_metadata = _font_info_handler->GetFontInfo().metadata;
		int calculated_height = _image_writer->Write(font_metadata.name, font_metadata.style, font_metadata.pointSize);
		_font_info_handler->Write(font_metadata.name, font_metadata.style, calculated_height, font_metadata.pointSize);

		// chronometer end
		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
		long long timetaken = std::chrono::duration_cast<std::chrono::milliseconds>(end - beg).count();

		// log
		LOGGER(info, "[font] generated font: " << font_metadata.name << ", style: " << font_metadata.style << ", size: " << font_metadata.pointSize << ", timetaken: " << timetaken << " ms");

		// create font data
		std::shared_ptr<FontData> fontData = std::make_shared<FontData>();
		fontData->fontInfo = std::move(_font_info_handler->CloneFontInfo());
		fontData->texture = std::move(_image_writer->CreateTexture());

		// reset
		_glyph_packer->Reset();
		//_image_writer->Reset();
		_font_info_handler->Reset();

		return fontData;
	}

	void FontGenerator::GenerateGlyph(int unicode, FT_Int32 load_flags, FT_Render_Mode render_mode)
	{
		// get index
		FT_UInt glyph_index = FT_Get_Char_Index(_face, unicode);
		if (glyph_index == 0)
		{
			if (unicode >= 32)
			{
				//LOGGER(error, "[font] error in get glyph: " << unicode);
			}
			_font_info_handler->AddGlyphData(unicode);
			return;
		}

		// load glyph
		_error = FT_Load_Glyph(_face, glyph_index, load_flags);
		if (_error)
		{
			LOGGER(error, "[font] error in load glyph: " << FT_Error_String(_error));
			return;
		}

		// slot
		FT_GlyphSlot slot = _face->glyph;

		// wh and offset
		int max_w = 0;
		int max_h = 0;
		int offset_x = 0;
		int offset_y = 0;
		int offset_main_x = 0;
		int offset_main_y = 0;

		// draw check
		bool draw_main_glyph = true;

		// get main glyph
		FT_Glyph glyph_main = nullptr;
		FT_Bitmap* bitmap_main = nullptr;
		FT_BitmapGlyph bitmap_glyph_main = nullptr;

		// get main glyph
		_error = FT_Get_Glyph(slot, &glyph_main);
		if (_error)
		{
			LOGGER(error, "[font] error in get glyph main: " << FT_Error_String(_error));
			return;
		}

		// render main glyph
		_error = FT_Glyph_To_Bitmap(&glyph_main, render_mode, /*origin*/ nullptr, /*destroy*/ true);
		if (_error)
		{
			LOGGER(error, "[font] error in glyph main to bitmap: " << FT_Error_String(_error));
			return;
		}

		// get main bitmap
		bitmap_glyph_main = reinterpret_cast<FT_BitmapGlyph>(glyph_main);
		bitmap_main = &bitmap_glyph_main->bitmap;

		// calculate wh and offset
		max_w = std::max(max_w, (int)bitmap_main->width);
		max_h = std::max(max_h, (int)bitmap_main->rows);
		offset_main_x = (int)bitmap_main->width;
		offset_main_y = (int)bitmap_main->rows;

		// hinting delta
		int delta_diff = std::round((_face->glyph->lsb_delta - _face->glyph->rsb_delta) / 64.0);

		// glyph metrics
		int width = std::round(slot->metrics.width / 64.0);
		int height = std::round(slot->metrics.height / 64.0);
		int advance = std::round(slot->metrics.horiAdvance / 64.0) + delta_diff;
		int bearing_x = std::round(slot->metrics.horiBearingX / 64.0) - offset_x;
		int bearing_y = std::round(slot->metrics.horiBearingY / 64.0) + offset_y;

		// wh and offset
		offset_main_x = std::abs(max_w - offset_main_x) / 2;
		offset_main_y = std::abs(max_h - offset_main_y) / 2;

		// pack
		int uv_w = max_w;
		int uv_h = max_h;
		std::pair<int, int> uv_lb = _glyph_packer->PackRect(unicode, uv_w, uv_h);
		int uv_l = uv_lb.first;
		int uv_b = uv_lb.second;

		// draw
		if (draw_main_glyph)
		{
			int l = uv_l + offset_x;
			int b = uv_b + offset_y;
			_image_writer->Draw(bitmap_main->buffer, bitmap_main->pitch, bitmap_main->width, bitmap_main->rows, l, b, DrawMode::draw_main);
		}

		// uv normalize, start from bottom left
		uv_b = _params.texture_height - (uv_b + uv_h);

		// add to font info
		_font_info_handler->AddGlyphData(
			unicode,
			uv_w, uv_h,
			advance,
			bearing_x, bearing_y,
			uv_l, uv_b, uv_w, uv_h
		);

		// cleanup
		if (draw_main_glyph)
		{
			//FT_Done_Glyph(glyph_main);
			//FT_Bitmap_Done(_library, bitmap_main);
		}
	}

	void FontGenerator::GetKerning(int unicode_l)
	{
		// get left glyph index
		FT_UInt left_glyph_index = FT_Get_Char_Index(_face, unicode_l);
		if (left_glyph_index == 0)
		{
			LOGGER(error, "[font] error in get char index for unicode_l: " << unicode_l);
			return;
		}

		// iterate over all unicodes
		for (int index = 0; index < _unicode_ranges.size(); index++)
		{
			// get right glyph index
			int unicode_r = _unicode_ranges.at(index);
			FT_UInt right_glyph_index = FT_Get_Char_Index(_face, unicode_r);
			if (right_glyph_index == 0)
			{
				LOGGER(error, "[font] error in get char index for unicode_r: " << unicode_r);
				continue;
			}

			// get kerning
			FT_Vector kerning{};
			_error = FT_Get_Kerning(_face, left_glyph_index, right_glyph_index, FT_KERNING_UNFITTED, &kerning); // FT_KERNING_DEFAULT, FT_KERNING_UNFITTED, FT_KERNING_UNSCALED
			if (_error)
			{
				LOGGER(error, "[font] error in get kerning: " << FT_Error_String(_error));
				return;
			}

			// add kerning if not zero
			if (kerning.x != 0 || kerning.y != 0)
			{
				_font_info_handler->AddKerningData(unicode_l, unicode_r, kerning.x, kerning.y);
			}
		}
	}

	void FontGenerator::FillUnicodeRange(int beg)
	{
		_unicode_ranges.push_back(beg);
	}

	void FontGenerator::FillUnicodeRangesInclusive(int beg, int end)
	{
		for (int i = beg; i <= end; i++)
		{
			FillUnicodeRange(i);
		}
	}

	FT_Vector FontGenerator::MakeVector(float delta_x, float delta_y)
	{
		/*
		16.16 format, 65536, variable << 16, (0x10000L)
		26.6  format,    64, variable << 6
		*/

		FT_Vector vector{};
		vector.x = delta_x * 64;
		vector.y = delta_y * 64;
		return vector;
	}

	FT_Matrix FontGenerator::MakeMatrix(float scale, float skew_in_rad)
	{
		FT_Matrix matrix{};
		matrix.xx = scale * 0x10000L;
		matrix.xy = -std::sin(skew_in_rad) * 0x10000L;
		matrix.yx = +std::sin(0.0) * 0x10000L;
		matrix.yy = scale * 0x10000L;
		return matrix;
	}

}
