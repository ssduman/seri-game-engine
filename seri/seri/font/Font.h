#pragma once

#include "seri/asset/AssetBase.h"
#include "seri/texture/TextureBase.h"

#include <string>
#include <memory>
#include <unordered_map>

namespace seri::font
{
	enum class CharacterSet
	{
		ascii,
		latin,
	};

	struct FontDesc
	{
		int pixelSize{ 48 };
		int atlasSize{ 512 };
		int padding{ 2 };
		bool kerning{ true };
		CharacterSet characterSet{ CharacterSet::latin };
	};

	struct Glyph
	{
		float width{ 0.0f };
		float height{ 0.0f };
		float advance{ 0.0f };
		float bearingX{ 0.0f };
		float bearingY{ 0.0f };
		float u0{ 0.0f };
		float v0{ 0.0f };
		float u1{ 0.0f };
		float v1{ 0.0f };
	};

	struct FontMetrics
	{
		float pixelSize{ 0.0f };
		float lineHeight{ 0.0f };
		float ascent{ 0.0f };
		float descent{ 0.0f };
	};

	class Font : public seri::asset::AssetBase
	{
	public:
		Font();

		bool Load(const std::string& path, const FontDesc& desc);

		const Glyph* GetGlyph(uint32_t unicode) const;
		float GetKerning(uint32_t left, uint32_t right) const;

		const FontMetrics& GetMetrics() const { return _metrics; }
		const FontDesc& GetDesc() const { return _desc; }
		const std::string& GetFamilyName() const { return _familyName; }
		const std::string& GetStyleName() const { return _styleName; }
		const std::shared_ptr<TextureBase>& GetAtlas() const { return _atlas; }
		size_t GetGlyphCount() const { return _glyphs.size(); }

	private:
		struct PendingGlyph
		{
			uint32_t unicode{ 0 };
			int width{ 0 };
			int height{ 0 };
			std::vector<unsigned char> bitmap{};
			Glyph glyph{};
		};


		static uint64_t MakeKerningKey(uint32_t left, uint32_t right);
		static void CollectCodepoints(CharacterSet characterSet, std::vector<uint32_t>& codepoints);

		FontDesc _desc{};
		FontMetrics _metrics{};
		std::string _familyName{};
		std::string _styleName{};
		std::unordered_map<uint32_t, Glyph> _glyphs{};
		std::unordered_map<uint64_t, float> _kernings{};
		std::shared_ptr<TextureBase> _atlas{};

	};
}
