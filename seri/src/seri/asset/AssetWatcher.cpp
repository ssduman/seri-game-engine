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
				//LOGGER(info, fmt::format("[asset_watcher] dir: '{}', file: '{}' added", dir, filename));
				break;
			case efsw::Actions::Delete:
				//LOGGER(info, fmt::format("[asset_watcher] dir: '{}', file: '{}' deleted", dir, filename));
				break;
			case efsw::Actions::Modified:
				//LOGGER(info, fmt::format("[asset_watcher] dir: '{}', file: '{}' modified", dir, filename));
				break;
			case efsw::Actions::Moved:
				//LOGGER(info, fmt::format("[asset_watcher] dir: '{}', file: '{}' moved to: '{}'", dir, oldFilename, filename));
				break;
		}
	}
}
