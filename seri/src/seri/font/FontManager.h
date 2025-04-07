#pragma once

#include "seri/logging/Logger.h"

#include "seri/font/FontGenerator.h"

#include <memory>
#include <vector>
#include <filesystem>

namespace seri::font
{
	class FontManager
	{
	public:
		FontManager(FontManager const&) = delete;

		void operator=(FontManager const&) = delete;

		static void Init(const char* fontFolderPath)
		{
			GetInstance();

			for (const auto& entry : std::filesystem::directory_iterator(fontFolderPath))
			{
				if (!entry.is_regular_file())
				{
					continue;
				}

				std::string fontName = entry.path().stem().string();
				LOGGER(info, "font found: " << fontName);

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

		static FontManager& GetInstance()
		{
			static FontManager instance;
			return instance;
		}

		std::vector<std::shared_ptr<FontData>>& GetPredefinedFonts()
		{
			return GetInstance()._predefinedFonts;
		}

	private:
		FontManager() = default;
		~FontManager() = default;

		std::vector<std::shared_ptr<FontData>> _predefinedFonts;

	};
}
