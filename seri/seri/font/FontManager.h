#pragma once

#include "seri/core/Singleton.h"

#include "seri/font/Font.h"

#include <memory>

namespace seri::font
{
	class FontManager : public seri::Singleton<FontManager>
	{
	public:
		static std::shared_ptr<Font> GetFont(uint64_t assetId);
		static std::shared_ptr<Font> GetDefaultFont();

	protected:
		friend struct seri::Singleton<FontManager>;

		FontManager() = default;
		~FontManager() = default;

	private:
		uint64_t _defaultFontId{ 0 };

	};
}
