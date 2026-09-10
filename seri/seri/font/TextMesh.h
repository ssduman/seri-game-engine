#pragma once

#include "seri/font/Font.h"

#include <glm/glm.hpp>

#include <string>

namespace seri
{
	class Mesh;
}

namespace seri::font
{
	enum class TextAlignH
	{
		left = 0,
		center = 1,
		right = 2,
	};

	enum class TextAlignV
	{
		top = 0,
		middle = 1,
		bottom = 2,
		baseline = 3,
	};

	inline const char* TextAlignHToString(TextAlignH alignH)
	{
		switch (alignH)
		{
			case TextAlignH::left: return "left";
			case TextAlignH::center: return "center";
			case TextAlignH::right: return "right";
			default: return "unknown";
		}
	}

	inline const char* TextAlignVToString(TextAlignV alignV)
	{
		switch (alignV)
		{
			case TextAlignV::top: return "top";
			case TextAlignV::middle: return "middle";
			case TextAlignV::baseline: return "baseline";
			case TextAlignV::bottom: return "bottom";
			default: return "unknown";
		}
	}

	struct TextBounds
	{
		float width{ 0.0f };
		float height{ 0.0f };
	};

	struct TextDesc
	{
		float fontSize{ 1.0f };
		float lineSpacing{ 1.0f };
		TextAlignH alignH{ TextAlignH::left };
		TextAlignV alignV{ TextAlignV::top };

		float GetLineOffsetX(float lineWidth) const
		{
			switch (alignH)
			{
				case TextAlignH::left:
					return 0.0f;
				case TextAlignH::center:
					return -lineWidth * 0.5f;
				case TextAlignH::right:
					return -lineWidth;
				default:
					return 0.0f;
			}
		}
	};

	struct TextMesh
	{
		static TextBounds BuildTextMesh(Mesh& mesh, const Font& font, const std::string& text, const TextDesc& desc);

	private:
		inline static const int kTabSize = 4;

		static float GetLineWidth(const Font& font, const std::vector<uint32_t>& line);

		static void SplitLines(const std::vector<uint32_t>& codepoints, std::vector<std::vector<uint32_t>>& lines);

		static float GetFirstBaseline(const Font& font, const TextDesc& desc, size_t lineCount, float lineAdvance);

	};
}
