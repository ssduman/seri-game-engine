#pragma once

#include "seri/core/Seri.h"

#include <FileWatch.hpp>

#include <filesystem>

namespace seri
{
	class AssetWatcher
	{
	public:
		AssetWatcher(const std::filesystem::path& watchPath)
		{
			_watcher = std::make_shared<filewatch::FileWatch<std::string>>(
				watchPath.string(), [](const std::filesystem::path& path, const filewatch::Event event)
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
				}
			);
		}

	private:
		std::shared_ptr<filewatch::FileWatch<std::string>> _watcher;

	};
}
