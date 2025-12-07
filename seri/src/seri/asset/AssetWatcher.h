#pragma once

#include "seri/util/Util.h"

#include <FileWatch.hpp>

#include <memory>
#include <string>
#include <filesystem>

namespace seri::asset
{
	class AssetWatcher
	{
	public:
		class AssetManager;

		AssetWatcher(const std::filesystem::path& watchPath);

	private:
		std::shared_ptr<filewatch::FileWatch<std::string>> _watcher;

	};
}
