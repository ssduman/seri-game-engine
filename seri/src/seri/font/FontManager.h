#pragma once

#include "seri/logging/Logger.h"

#include "seri/font/FontGenerator.h"

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

			FontGenerator fontGenerator{ FontGeneratorParams{} };
			fontGenerator.Init(FontGeneratorInitParams{});
			fontGenerator.Generate();

			for (const auto& entry : std::filesystem::directory_iterator(fontFolderPath))
			{
				std::string name = entry.path().stem().string();
				LOGGER(info, "font found: " << name);
			}
		}

		static FontManager& GetInstance()
		{
			static FontManager instance;
			return instance;
		}

	private:
		FontManager() = default;
		~FontManager() = default;

		std::vector<std::string> _predefinedFonts;

	};
}
