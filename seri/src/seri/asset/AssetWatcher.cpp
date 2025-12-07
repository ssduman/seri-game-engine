#include "Seripch.h"

#include "seri/asset/AssetManager.h"

namespace seri::asset
{
	AssetWatcher::AssetWatcher(const std::filesystem::path& watchPath)
	{
		_watcher = std::make_shared<filewatch::FileWatch<std::string>>(
			watchPath.string(), [&](const std::filesystem::path& path, const filewatch::Event event)
			{
				LOGGER(info, fmt::format("[asset_watcher] path: {}, change: {}", path.string(), filewatch::event_to_string(event)));

				switch (event)
				{
					case filewatch::Event::added:
						break;
					case filewatch::Event::removed:
						break;
					case filewatch::Event::modified:
						break;
					case filewatch::Event::renamed_old:
						break;
					case filewatch::Event::renamed_new:
						break;
				};

				seri::asset::AssetManager::GetInstance().UpdateAssetTree();
			}
		);
	}

}
