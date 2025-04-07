#include "Seripch.h"

#include "seri/logging/Logger.h"

#include "FontInfoHandler.h"

namespace seri::font
{
	void to_json(nlohmann::ordered_json& j, const GlyphData& p)
	{
		j = nlohmann::ordered_json{
			{ "unicode", p.unicode },
			{ "width", p.width },
			{ "height", p.height },
			{ "advance", p.advance },
			{ "bearingX", p.bearingX },
			{ "bearingY", p.bearingY },
			{ "uvL", p.uvL },
			{ "uvB", p.uvB },
			{ "uvW", p.uvW },
			{ "uvH", p.uvH },
		};
	}

	void from_json(const nlohmann::ordered_json& j, GlyphData& p)
	{
		j.at("unicode").get_to(p.unicode);
		j.at("width").get_to(p.width);
		j.at("height").get_to(p.height);
		j.at("advance").get_to(p.advance);
		j.at("bearingX").get_to(p.bearingX);
		j.at("bearingY").get_to(p.bearingY);
		j.at("uvL").get_to(p.uvL);
		j.at("uvB").get_to(p.uvB);
		j.at("uvW").get_to(p.uvW);
		j.at("uvH").get_to(p.uvH);
	}

	void to_json(nlohmann::ordered_json& j, const KerningData& p)
	{
		j = nlohmann::ordered_json{
			{ "unicode_l", p.unicode_l },
			{ "unicode_r", p.unicode_r },
			{ "x", p.x },
			{ "y", p.y },
		};
	}

	void from_json(const nlohmann::ordered_json& j, KerningData& p)
	{
		j.at("unicode_l").get_to(p.unicode_l);
		j.at("unicode_r").get_to(p.unicode_r);
		j.at("x").get_to(p.x);
		j.at("y").get_to(p.y);
	}

	void to_json(nlohmann::ordered_json& j, const FontMetadata& p)
	{
		j = nlohmann::ordered_json{
			{ "name", p.name },
			{ "style", p.style },
			{ "renderMode", p.renderMode },
			{ "count", p.count },
			{ "atlasW", p.atlasW },
			{ "atlasH", p.atlasH },
			{ "pointSize", p.pointSize },
			{ "lineHeight", p.lineHeight },
			{ "ascentLine", p.ascentLine },
			{ "descentLine", p.descentLine },
		};
	}

	void from_json(const nlohmann::ordered_json& j, FontMetadata& p)
	{
		j.at("name").get_to(p.name);
		j.at("style").get_to(p.style);
		j.at("renderMode").get_to(p.renderMode);
		j.at("count").get_to(p.count);
		j.at("atlasW").get_to(p.atlasW);
		j.at("atlasH").get_to(p.atlasH);
		j.at("pointSize").get_to(p.pointSize);
		j.at("lineHeight").get_to(p.lineHeight);
		j.at("ascentLine").get_to(p.ascentLine);
		j.at("descentLine").get_to(p.descentLine);
	}

	void to_json(nlohmann::ordered_json& j, const FontInfo& p)
	{
		j = nlohmann::ordered_json{
			{ "metadata", p.metadata },
			{ "glyphDataList", p.glyph_data_list },
			{ "kerningDataList", p.kerning_data_list },
		};
	}

	void from_json(const nlohmann::ordered_json& j, FontInfo& p)
	{
		j.at("metadata").get_to(p.metadata);
		j.at("glyphDataList").get_to(p.glyph_data_list);
		j.at("kerningDataList").get_to(p.kerning_data_list);
	}

	FontInfoHandler::FontInfoHandler(FontInfoHandlerParams params)
		: _params{ params }
	{
		std::filesystem::path root{ _params.output_folder_path };
		std::filesystem::path folder{ get_pretty_font_name(_params.font_name) + "_" + output_folder_name };
		output_path = root / folder;
		if (!std::filesystem::is_directory(output_path) || !std::filesystem::exists(output_path))
		{
			std::filesystem::create_directory(output_path);
		}

		Reset();
	}

	void FontInfoHandler::Reset()
	{
		_font_info = FontInfo();
	}

	void FontInfoHandler::PutFontMetada(
		std::string name, std::string style, std::string render_mode_str,
		int count, int texture_width, int texture_height, int pixel_size,
		int height, int ascender, int descender
	)
	{
		_font_info.metadata = FontMetadata();
		_font_info.metadata.name = name;
		_font_info.metadata.style = style;
		_font_info.metadata.renderMode = render_mode_str;
		_font_info.metadata.count = count;
		_font_info.metadata.atlasW = texture_width;
		_font_info.metadata.atlasH = texture_height;
		_font_info.metadata.pointSize = pixel_size;
		_font_info.metadata.lineHeight = height;
		_font_info.metadata.ascentLine = ascender;
		_font_info.metadata.descentLine = descender;

		_font_info.glyph_data_list = std::vector<GlyphData>();
		_font_info.glyph_data_list.reserve(count);

		_font_info.kerning_data_list = std::vector<KerningData>();
		_font_info.kerning_data_list.reserve(count * count);
	}

	void FontInfoHandler::AddGlyphData(int unicode)
	{
		GlyphData glyph_data{};
		glyph_data.unicode = unicode;
		_font_info.glyph_data_list.push_back(glyph_data);
	}

	void FontInfoHandler::AddGlyphData(int unicode, int width, int height, int advance, int bearingX, int bearingY, int uvL, int uvB, int uvW, int uvH)
	{
		GlyphData glyph_data{};
		glyph_data.unicode = unicode;
		glyph_data.width = width;
		glyph_data.height = height;
		glyph_data.advance = advance;
		glyph_data.bearingX = bearingX;
		glyph_data.bearingY = bearingY;
		glyph_data.uvL = uvL;
		glyph_data.uvB = uvB;
		glyph_data.uvW = uvW;
		glyph_data.uvH = uvH;
		_font_info.glyph_data_list.push_back(glyph_data);
	}

	void FontInfoHandler::AddKerningData(int unicode_l, int unicode_r, int x, int y)
	{
		KerningData kerning_data{};
		kerning_data.unicode_l = unicode_l;
		kerning_data.unicode_r = unicode_r;
		kerning_data.x = x;
		kerning_data.y = y;
		_font_info.kerning_data_list.push_back(kerning_data);
	}

	FontInfo FontInfoHandler::GetFontInfo()
	{
		return _font_info;
	}

	std::string FontInfoHandler::Dump(int indent)
	{
		return (nlohmann::ordered_json{ _font_info })[0].dump(indent);
	}

	void FontInfoHandler::Write(std::string name, std::string style, int calculated_height, int pixel_size, int indent)
	{
		_font_info.metadata.atlasH = calculated_height;

		std::string pretty_name = get_pretty_name(name, style, pixel_size);
		std::string write_path = (output_path / std::filesystem::path{ pretty_name + ".json" }).string();
		std::ofstream json_dump_stream{ write_path.c_str() };
		json_dump_stream << Dump(indent) << std::endl;
		json_dump_stream.close();
	}
}
