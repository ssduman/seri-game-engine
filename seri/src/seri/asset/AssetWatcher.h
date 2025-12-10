#pragma once

#include "seri/util/Util.h"

#include <FileWatch.hpp>
#include <efsw/efsw.hpp>

#include <memory>
#include <string>
#include <filesystem>

namespace seri::asset
{
	class AssetWatcher : public efsw::FileWatchListener
	{
	public:
		class AssetManager;

		AssetWatcher(const std::filesystem::path& watchPath);

		void handleFileAction(efsw::WatchID watchid, const std::string& dir, const std::string& filename, efsw::Action action, std::string oldFilename) override;

	};

	class AssetWatcher2
	{
	public:
		class AssetManager;

		AssetWatcher2(const std::filesystem::path& watchPath);

		void Start();

	private:
		std::filesystem::path _watchPath;
		std::shared_ptr<filewatch::FileWatch<std::string>> _watcher;

	};

}
