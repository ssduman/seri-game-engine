#pragma once

#include "seri/logging/Logger.h"

#include "seri/font/FontGenerator.h"

#include <memory>
#include <vector>
#include <filesystem>

class seri::Mesh;

namespace seri::font
{
	class FontManager
	{
	public:
		FontManager(FontManager const&) = delete;

		void operator=(FontManager const&) = delete;

		static void Init(const char* fontFolderPath);

		static FontManager& GetInstance()
		{
			static FontManager instance;
			return instance;
		}

		static std::vector<std::shared_ptr<FontData>>& GetPredefinedFonts()
		{
			return GetInstance()._predefinedFonts;
		}

		static void MakeText(std::unique_ptr<seri::Mesh>& mesh, const FontInfo& fontInfo, std::string text, float posX, float posY);
		static void MakeText(std::unique_ptr<seri::Mesh>& mesh, int fontIndex, std::string text, float posX, float posY);

	private:
		FontManager() = default;
		~FontManager() = default;

		std::vector<std::shared_ptr<FontData>> _predefinedFonts;

	};
}
