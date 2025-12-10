#include "Seripch.h"

#include "seri/asset/AssetManager.h"

namespace seri::asset
{
	AssetWatcher::AssetWatcher(const std::filesystem::path& watchPath)
	{
	}

	void AssetWatcher::handleFileAction(efsw::WatchID watchid, const std::string& dir, const std::string& filename, efsw::Action action, std::string oldFilename)
	{
		switch (action)
		{
			case efsw::Actions::Add:
				LOGGER(info, fmt::format("[asset_watcher] dir: '{}', file: '{}' added", dir, filename));
				break;
			case efsw::Actions::Delete:
				LOGGER(info, fmt::format("[asset_watcher] dir: '{}', file: '{}' deleted", dir, filename));
				break;
			case efsw::Actions::Modified:
				LOGGER(info, fmt::format("[asset_watcher] dir: '{}', file: '{}' modified", dir, filename));
				break;
			case efsw::Actions::Moved:
				LOGGER(info, fmt::format("[asset_watcher] dir: '{}', file: '{}' moved to: '{}'", dir, oldFilename, filename));
				break;
		}
	}

	AssetWatcher2::AssetWatcher2(const std::filesystem::path& watchPath) : _watchPath(watchPath)
	{
	}

	void AssetWatcher2::Start()
	{
		LOGGER(info, fmt::format("[asset_watcher] starting watch for path: '{}'", _watchPath.string()));

		_watcher = std::make_shared<filewatch::FileWatch<std::string>>(
			_watchPath.string(), [&](const std::filesystem::path& path, const filewatch::Event event)
			{
				LOGGER(info, fmt::format("[asset_watcher] path: '{}', change: '{}'", path.string(), filewatch::event_to_string(event)));

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

				//seri::asset::AssetManager::GetInstance().UpdateAssetTree();
			}
		);
	}

}
