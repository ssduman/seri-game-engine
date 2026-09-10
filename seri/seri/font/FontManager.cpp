#include "Seripch.h"

#include "seri/font/FontManager.h"
#include "seri/asset/AssetManager.h"
#include "seri/core/Literals.h"

#include <algorithm>

namespace seri::font
{
	std::shared_ptr<Font> FontManager::GetFont(uint64_t assetId)
	{
		if (assetId != 0)
		{
			if (std::shared_ptr<Font> font = seri::asset::AssetManager::GetAssetByID<Font>(assetId))
			{
				return font;
			}
		}

		return GetDefaultFont();
	}

	std::shared_ptr<Font> FontManager::GetDefaultFont()
	{
		uint64_t& defaultFontId = GetInstance()._defaultFontId;

		if (defaultFontId != 0)
		{
			if (std::shared_ptr<Font> font = seri::asset::AssetManager::GetAssetByID<Font>(defaultFontId))
			{
				return font;
			}
		}

		std::vector<seri::asset::AssetMetadata> fonts = seri::asset::AssetManager::GetAssetsByType(seri::asset::AssetType::font);
		if (fonts.empty())
		{
			return nullptr;
		}

		std::sort(
			fonts.begin(),
			fonts.end(),
			[](const seri::asset::AssetMetadata& lhs, const seri::asset::AssetMetadata& rhs)
			{
				return lhs.name < rhs.name;
			}
		);

		auto it = std::find_if(
			fonts.begin(),
			fonts.end(),
			[](const seri::asset::AssetMetadata& metadata)
			{
				return metadata.name == seri::literals::kDefaultFontName;
			}
		);

		defaultFontId = it != fonts.end() ? it->id : fonts.front().id;

		return seri::asset::AssetManager::GetAssetByID<Font>(defaultFontId);
	}

}
