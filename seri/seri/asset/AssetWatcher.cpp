#include "Seripch.h"

#include "seri/asset/AssetManager.h"

namespace seri::asset
{
	AssetWatcher::AssetWatcher(const std::filesystem::path& watchPath)
	{
	}

	void AssetWatcher::handleFileAction(efsw::WatchID watchid, const std::string& dir, const std::string& filename, efsw::Action action, const std::string& oldFilename)
	{
		std::filesystem::path path = std::filesystem::path(dir) / filename;

		std::string extension = path.extension().string();
		if (!extension.empty() && extension[0] == '.')
		{
			extension.erase(0, 1);
		}
		seri::Util::ToLower(extension);

		if (extension == AssetManager::GetInstance().kAssetMetaExtension || extension == "tmp")
		{
			return;
		}

		if (!filename.empty() && (filename[0] == '~' || filename[0] == '.'))
		{
			return;
		}

		switch (action)
		{
			case efsw::Actions::Add:
				LIB_LOGGER(verbose, asset_watcher) << "file at path: '" << path.string() << "' added";
				break;
			case efsw::Actions::Delete:
				LIB_LOGGER(verbose, asset_watcher) << "file at path: '" << path.string() << "' deleted";
				break;
			case efsw::Actions::Modified:
				LIB_LOGGER(verbose, asset_watcher) << "file at path: '" << path.string() << "' modified";
				break;
			case efsw::Actions::Moved:
				LIB_LOGGER(verbose, asset_watcher) << "file at path: '" << path.string() << "' moved from '" << oldFilename << "'";
				break;
		}

		AssetManager::NotifyFileEvent(path);
	}
}
