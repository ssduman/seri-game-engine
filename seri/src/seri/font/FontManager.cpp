#include "Seripch.h"

#include "seri/graphic/Mesh.h"
#include "seri/font/FontManager.h"

namespace seri::font
{
	void FontManager::Init(const char* fontFolderPath)
	{
		GetInstance();

		for (const auto& entry : std::filesystem::directory_iterator(fontFolderPath))
		{
			if (!entry.is_regular_file())
			{
				continue;
			}

			if (entry.path().extension() != ".ttf" && entry.path().extension() != ".otf")
			{
				continue;
			}

			std::string fontName = entry.path().stem().string();
			LOGGER(info, "[font] font found: " << fontName);

			FontGeneratorParams genParams{};
			FontGeneratorInitParams initParams{};

			genParams.font_name = fontName;

			FontGenerator fontGenerator{ genParams };
			fontGenerator.Init(initParams);
			std::shared_ptr fontData = std::move(fontGenerator.Generate());

			GetInstance()._predefinedFonts.push_back(std::move(fontData));
		}

		auto& instance = GetInstance();
	}

	void FontManager::MakeText(std::shared_ptr<seri::Mesh>& mesh, const FontInfo& fontInfo, std::string text, float posX, float posY)
	{
		mesh->Clear();

		uint32_t currOffset = 0;
		for (auto& c : text)
		{
			uint32_t unicode = static_cast<uint32_t>(c);

			for (const GlyphData& glyph : fontInfo.glyph_data_list)
			{
				if (glyph.unicode == unicode)
				{
					float altasW = fontInfo.metadata.atlasW;
					float altasH = fontInfo.metadata.atlasH;

					float uvL = (glyph.uvL) / altasW;
					float uvB = (altasH - glyph.uvB - glyph.uvH) / altasH;
					float uvW = glyph.uvW / altasW;
					float uvH = glyph.uvH / altasH;

					float tempPosX = posX + glyph.bearingX;
					float tempPosY = posY + glyph.bearingY;

					glm::vec3 v0 = {
						tempPosX,
						tempPosY,
						0.0f
					};
					glm::vec3 v1 = {
						tempPosX + glyph.width,
						tempPosY,
						0.0f
					};
					glm::vec3 v2 = {
						tempPosX + glyph.width,
						tempPosY - glyph.height,
						0.0f
					};
					glm::vec3 v3 = {
						tempPosX,
						tempPosY - glyph.height,
						0.0f
					};

					glm::vec2 uv0 = {
						uvL,
						uvB,
					};
					glm::vec2 uv1 = {
						uvL + uvW,
						uvB,
					};
					glm::vec2 uv2 = {
						uvL + uvW,
						uvB + uvH,
					};
					glm::vec2 uv3 = {
						uvL,
						uvB + uvH,
					};

					mesh->vertices.push_back(v0);
					mesh->vertices.push_back(v1);
					mesh->vertices.push_back(v2);
					mesh->vertices.push_back(v3);

					mesh->uv0s.push_back(uv0);
					mesh->uv0s.push_back(uv1);
					mesh->uv0s.push_back(uv2);
					mesh->uv0s.push_back(uv3);

					mesh->indices.push_back(currOffset + 0);
					mesh->indices.push_back(currOffset + 1);
					mesh->indices.push_back(currOffset + 3);
					mesh->indices.push_back(currOffset + 1);
					mesh->indices.push_back(currOffset + 2);
					mesh->indices.push_back(currOffset + 3);

					currOffset += 4;
					posX += glyph.advance;

					break;
				}
			}
		}

		//mesh->Build();
		mesh->Update();
	}

	void FontManager::MakeText(std::shared_ptr<seri::Mesh>& mesh, int fontIndex, std::string text, float posX, float posY)
	{
		const auto& fontInfo = FontManager::GetPredefinedFonts()[fontIndex]->fontInfo;
		const auto& fontTexture = FontManager::GetPredefinedFonts()[fontIndex]->texture;

		MakeText(mesh, fontInfo, text, posX, posY);
	}
}
