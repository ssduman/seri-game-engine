#include "Seripch.h"

#include "seri/font/Font.h"
#include "seri/logging/Logger.h"

#include "seri/third_party/freetype.h"

#include <stb_rect_pack.h>

#include <vector>
#include <algorithm>

namespace seri::font
{
	Font::Font()
	{
		id = seri::Random::UUID();
		type = seri::asset::AssetType::font;
	}

	bool Font::Load(const std::string& path, const FontDesc& desc)
	{
		_desc = desc;
		_glyphs.clear();
		_kernings.clear();
		_atlas = nullptr;

		FT_Library library = nullptr;
		FT_Error error = FT_Init_FreeType(&library);
		if (error)
		{
			LIB_LOGGER(error, font) << "init freetype failed: " << FT_Error_String(error);
			return false;
		}

		FT_Face face = nullptr;
		error = FT_New_Face(library, path.c_str(), 0, &face);
		if (error)
		{
			LIB_LOGGER(error, font) << "loading face '" << path << "' failed: " << FT_Error_String(error);
			FT_Done_FreeType(library);
			return false;
		}

		error = FT_Select_Charmap(face, FT_ENCODING_UNICODE);
		if (error)
		{
			LIB_LOGGER(error, font) << "selecting unicode charmap failed: " << FT_Error_String(error);
			FT_Done_Face(face);
			FT_Done_FreeType(library);
			return false;
		}

		error = FT_Set_Pixel_Sizes(face, 0, _desc.pixelSize);
		if (error)
		{
			LIB_LOGGER(error, font) << "setting pixel size failed: " << FT_Error_String(error);
			FT_Done_Face(face);
			FT_Done_FreeType(library);
			return false;
		}

		_familyName = face->family_name ? face->family_name : "";
		_styleName = face->style_name ? face->style_name : "";

		_metrics.pixelSize = static_cast<float>(_desc.pixelSize);
		_metrics.lineHeight = face->size->metrics.height / 64.0f;
		_metrics.ascent = face->size->metrics.ascender / 64.0f;
		_metrics.descent = face->size->metrics.descender / 64.0f;

		std::vector<uint32_t> codepoints;
		CollectCodepoints(_desc.characterSet, codepoints);

		std::vector<PendingGlyph> pendings;
		pendings.reserve(codepoints.size());

		for (uint32_t codepoint : codepoints)
		{
			if (FT_Get_Char_Index(face, codepoint) == 0)
			{
				continue;
			}

			error = FT_Load_Char(face, codepoint, FT_LOAD_RENDER | FT_LOAD_TARGET_NORMAL);
			if (error)
			{
				continue;
			}

			FT_GlyphSlot slot = face->glyph;

			PendingGlyph pending{};
			pending.unicode = codepoint;
			pending.width = static_cast<int>(slot->bitmap.width);
			pending.height = static_cast<int>(slot->bitmap.rows);
			pending.glyph.width = static_cast<float>(pending.width);
			pending.glyph.height = static_cast<float>(pending.height);
			pending.glyph.advance = slot->advance.x / 64.0f;
			pending.glyph.bearingX = static_cast<float>(slot->bitmap_left);
			pending.glyph.bearingY = static_cast<float>(slot->bitmap_top);

			if (pending.width > 0 && pending.height > 0)
			{
				pending.bitmap.resize(static_cast<size_t>(pending.width) * pending.height);
				for (int row = 0; row < pending.height; row++)
				{
					const unsigned char* src = slot->bitmap.buffer + static_cast<ptrdiff_t>(row) * slot->bitmap.pitch;
					std::copy(src, src + pending.width, pending.bitmap.begin() + static_cast<size_t>(row) * pending.width);
				}
			}

			pendings.push_back(std::move(pending));
		}

		if (pendings.empty())
		{
			LIB_LOGGER(error, font) << "no glyph loaded from '" << path << "'";
			FT_Done_Face(face);
			FT_Done_FreeType(library);
			return false;
		}

		std::vector<stbrp_rect> rects;
		rects.reserve(pendings.size());
		for (size_t i = 0; i < pendings.size(); i++)
		{
			if (pendings[i].width <= 0 || pendings[i].height <= 0)
			{
				continue;
			}

			stbrp_rect rect{};
			rect.id = static_cast<int>(i);
			rect.w = pendings[i].width + _desc.padding;
			rect.h = pendings[i].height + _desc.padding;
			rects.push_back(rect);
		}

		const int maxAtlasSize = 4096;
		int atlasSize = std::max(_desc.atlasSize, 64);
		bool packed = false;

		while (!packed)
		{
			std::vector<stbrp_node> nodes(atlasSize);
			stbrp_context ctx{};
			stbrp_init_target(&ctx, atlasSize, atlasSize, nodes.data(), static_cast<int>(nodes.size()));

			packed = stbrp_pack_rects(&ctx, rects.data(), static_cast<int>(rects.size())) != 0;

			if (packed || atlasSize >= maxAtlasSize)
			{
				break;
			}

			atlasSize *= 2;
		}

		if (!packed)
		{
			LIB_LOGGER(error, font) << "packing glyphs of '" << path << "' failed at atlas size " << atlasSize;
			FT_Done_Face(face);
			FT_Done_FreeType(library);
			return false;
		}

		size_t atlasBytes = static_cast<size_t>(atlasSize) * atlasSize;
		unsigned char* atlasBuffer = static_cast<unsigned char*>(calloc(atlasBytes, 1));
		if (!atlasBuffer)
		{
			LIB_LOGGER(error, font) << "allocating atlas of size " << atlasSize << " failed";
			FT_Done_Face(face);
			FT_Done_FreeType(library);
			return false;
		}

		float atlasSizeF = static_cast<float>(atlasSize);

		for (const stbrp_rect& rect : rects)
		{
			PendingGlyph& pending = pendings[rect.id];

			for (int row = 0; row < pending.height; row++)
			{
				unsigned char* dst = atlasBuffer + (static_cast<size_t>(rect.y) + row) * atlasSize + rect.x;
				const unsigned char* src = pending.bitmap.data() + static_cast<size_t>(row) * pending.width;
				std::copy(src, src + pending.width, dst);
			}

			pending.glyph.u0 = rect.x / atlasSizeF;
			pending.glyph.v0 = rect.y / atlasSizeF;
			pending.glyph.u1 = (rect.x + pending.width) / atlasSizeF;
			pending.glyph.v1 = (rect.y + pending.height) / atlasSizeF;
		}

		_glyphs.reserve(pendings.size());
		for (const PendingGlyph& pending : pendings)
		{
			_glyphs[pending.unicode] = pending.glyph;
		}

		if (_desc.kerning && FT_HAS_KERNING(face))
		{
			for (const PendingGlyph& left : pendings)
			{
				FT_UInt leftIndex = FT_Get_Char_Index(face, left.unicode);

				for (const PendingGlyph& right : pendings)
				{
					FT_UInt rightIndex = FT_Get_Char_Index(face, right.unicode);

					FT_Vector kerning{};
					if (FT_Get_Kerning(face, leftIndex, rightIndex, FT_KERNING_DEFAULT, &kerning) != 0)
					{
						continue;
					}

					if (kerning.x == 0)
					{
						continue;
					}

					_kernings[MakeKerningKey(left.unicode, right.unicode)] = kerning.x / 64.0f;
				}
			}
		}

		FT_Done_Face(face);
		FT_Done_FreeType(library);

		TextureDesc textureDesc{};
		textureDesc.flip = false;
		textureDesc.format = TextureFormat::red__red8ubyte;

		_atlas = TextureBase::Create();
		_atlas->Init(textureDesc, atlasBuffer, atlasSize, atlasSize, 1);

		LIB_LOGGER(info, font) << "loaded font: " << _familyName << " " << _styleName
			<< ", size: " << _desc.pixelSize
			<< ", glyphs: " << _glyphs.size()
			<< ", kernings: " << _kernings.size()
			<< ", atlas: " << atlasSize << "x" << atlasSize;

		return true;
	}

	const Glyph* Font::GetGlyph(uint32_t unicode) const
	{
		auto it = _glyphs.find(unicode);
		return it != _glyphs.end() ? &it->second : nullptr;
	}

	float Font::GetKerning(uint32_t left, uint32_t right) const
	{
		auto it = _kernings.find(MakeKerningKey(left, right));
		return it != _kernings.end() ? it->second : 0.0f;
	}

	void Font::CollectCodepoints(CharacterSet characterSet, std::vector<uint32_t>& codepoints)
	{
		auto addRange = [&codepoints](uint32_t beg, uint32_t end)
			{
				for (uint32_t cp = beg; cp <= end; cp++)
				{
					codepoints.push_back(cp);
				}
			};

		addRange(0x0020, 0x007E);

		if (characterSet == CharacterSet::latin)
		{
			addRange(0x00A0, 0x00FF);
			addRange(0x0100, 0x017F);
		}
	}

	uint64_t Font::MakeKerningKey(uint32_t left, uint32_t right)
	{
		return (static_cast<uint64_t>(left) << 32) | static_cast<uint64_t>(right);
	}

}
