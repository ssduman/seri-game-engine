#include "Seripch.h"

#include "seri/util/Util.h"
#include "seri/font/TextMesh.h"
#include "seri/graphic/Mesh.h"

#include <vector>

namespace seri::font
{
	TextBounds TextMesh::BuildTextMesh(Mesh& mesh, const Font& font, const std::string& text, const TextDesc& desc)
	{
		mesh.Clear();

		const FontMetrics& metrics = font.GetMetrics();

		if (metrics.pixelSize <= 0.0f)
		{
			return {};
		}

		std::vector<uint32_t> codepoints;
		Util::DecodeUTF8(text, codepoints);

		std::vector<std::vector<uint32_t>> lines;
		SplitLines(codepoints, lines);

		mesh.vertices.reserve(codepoints.size() * 4);
		mesh.uv0s.reserve(codepoints.size() * 4);
		mesh.indices.reserve(codepoints.size() * 6);

		float scale = desc.fontSize / metrics.pixelSize;
		float lineAdvance = metrics.lineHeight * desc.lineSpacing;
		float baseline = GetFirstBaseline(font, desc, lines.size(), lineAdvance);

		const Glyph* space = font.GetGlyph(' ');

		float maxWidth = 0.0f;

		for (const auto& line : lines)
		{
			float penX = desc.GetLineOffsetX(GetLineWidth(font, line));
			float lineBeg = penX;
			uint32_t prev = 0;

			for (uint32_t codepoint : line)
			{
				if (codepoint == '\t')
				{
					penX += space ? space->advance * kTabSize : 0.0f;
					prev = 0;
					continue;
				}

				const Glyph* glyph = font.GetGlyph(codepoint);
				if (!glyph)
				{
					continue;
				}

				if (prev != 0)
				{
					penX += font.GetKerning(prev, codepoint);
				}

				if (glyph->width > 0.0f && glyph->height > 0.0f)
				{
					float left = (penX + glyph->bearingX) * scale;
					float right = left + glyph->width * scale;
					float top = (baseline + glyph->bearingY) * scale;
					float bottom = top - glyph->height * scale;

					uint32_t offset = static_cast<uint32_t>(mesh.vertices.size());

					mesh.vertices.push_back({ left, bottom, 0.0f });
					mesh.vertices.push_back({ right, bottom, 0.0f });
					mesh.vertices.push_back({ right, top, 0.0f });
					mesh.vertices.push_back({ left, top, 0.0f });

					mesh.uv0s.push_back({ glyph->u0, glyph->v1 });
					mesh.uv0s.push_back({ glyph->u1, glyph->v1 });
					mesh.uv0s.push_back({ glyph->u1, glyph->v0 });
					mesh.uv0s.push_back({ glyph->u0, glyph->v0 });

					mesh.indices.push_back(offset + 0);
					mesh.indices.push_back(offset + 1);
					mesh.indices.push_back(offset + 3);
					mesh.indices.push_back(offset + 1);
					mesh.indices.push_back(offset + 2);
					mesh.indices.push_back(offset + 3);
				}

				penX += glyph->advance;
				prev = codepoint;
			}

			maxWidth = std::max(maxWidth, penX - lineBeg);
			baseline -= lineAdvance;
		}

		if (mesh.vertices.empty() && !mesh.GetVao())
		{
			return {};
		}

		mesh.Update();

		float height = static_cast<float>(lines.size() - 1) * lineAdvance + metrics.ascent - metrics.descent;

		return { maxWidth * scale, height * scale };
	}

	float TextMesh::GetLineWidth(const Font& font, const std::vector<uint32_t>& line)
	{
		const Glyph* space = font.GetGlyph(' ');

		float penX = 0.0f;
		uint32_t prev = 0;

		for (uint32_t codepoint : line)
		{
			if (codepoint == '\t')
			{
				penX += space ? space->advance * kTabSize : 0.0f;
				prev = 0;
				continue;
			}

			const Glyph* glyph = font.GetGlyph(codepoint);
			if (!glyph)
			{
				continue;
			}

			if (prev != 0)
			{
				penX += font.GetKerning(prev, codepoint);
			}

			penX += glyph->advance;
			prev = codepoint;
		}

		return penX;
	}

	void TextMesh::SplitLines(const std::vector<uint32_t>& codepoints, std::vector<std::vector<uint32_t>>& lines)
	{
		lines.emplace_back();

		for (uint32_t codepoint : codepoints)
		{
			if (codepoint == '\r')
			{
				continue;
			}

			if (codepoint == '\n')
			{
				lines.emplace_back();
				continue;
			}

			lines.back().push_back(codepoint);
		}
	}

	float TextMesh::GetFirstBaseline(const Font& font, const TextDesc& desc, size_t lineCount, float lineAdvance)
	{
		const FontMetrics& metrics = font.GetMetrics();

		float span = static_cast<float>(lineCount - 1) * lineAdvance;

		switch (desc.alignV)
		{
			case TextAlignV::top:
				return -metrics.ascent;
			case TextAlignV::middle:
				return (span - metrics.ascent - metrics.descent) * 0.5f;
			case TextAlignV::bottom:
				return span - metrics.descent;
			case TextAlignV::baseline:
				return 0.0f;
			default:
				return 0.0f;
		}
	}

}
