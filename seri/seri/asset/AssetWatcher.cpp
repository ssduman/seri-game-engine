#include "Seripch.h"

#include "seri/asset/AssetManager.h"

namespace seri::asset
{
	AssetWatcher::AssetWatcher(const std::filesystem::path& watchPath)
	{
	}

	void AssetWatcher::handleFileAction(efsw::WatchID watchid, const std::string& dir, const std::string& filename, efsw::Action action, const std::string& oldFilename)
	{
		switch (action)
		{
			case efsw::Actions::Add:
				//LIB_LOGGER(info, asset_watcher) << fmt::format("dir: '{}', file: '{}' added", dir, filename);
				break;
			case efsw::Actions::Delete:
				//LIB_LOGGER(info, asset_watcher) << fmt::format("dir: '{}', file: '{}' deleted", dir, filename);
				break;
			case efsw::Actions::Modified:
				//LIB_LOGGER(info, asset_watcher) << fmt::format("dir: '{}', file: '{}' modified", dir, filename);
				break;
			case efsw::Actions::Moved:
				//LIB_LOGGER(info, asset_watcher) << fmt::format("dir: '{}', file: '{}' moved to: '{}'", dir, oldFilename, filename);
				break;
		}
	}
}
