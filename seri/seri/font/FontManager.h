#pragma once

#include "seri/core/Singleton.h"

#include "seri/logging/Logger.h"

#include "seri/font/FontGenerator.h"

#include <memory>
#include <vector>
#include <filesystem>

class seri::Mesh;

namespace seri::font
{
	class FontManager : public seri::Singleton<FontManager>
	{
	public:
		static void Init(const char* fontFolderPath);

		static std::vector<std::shared_ptr<FontData>>& GetPredefinedFonts()
		{
			return GetInstance()._predefinedFonts;
		}

		static void MakeText(std::shared_ptr<seri::Mesh>& mesh, const FontInfo& fontInfo, std::string text, float posX, float posY);
		static void MakeText(std::shared_ptr<seri::Mesh>& mesh, int fontIndex, std::string text, float posX, float posY);

	protected:
		friend struct seri::Singleton<FontManager>;

		FontManager() = default;
		~FontManager() = default;

	private:
		std::vector<std::shared_ptr<FontData>> _predefinedFonts;

	};
}
