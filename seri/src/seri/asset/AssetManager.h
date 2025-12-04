#pragma once

#include "seri/core/Seri.h"
#include "seri/util/Util.h"
#include "seri/scene/Scene.h"
#include "seri/texture/Skybox.h"
#include "seri/component/Components.h"

#include <entt/entt.hpp>
#include <yaml-cpp/yaml.h>

#include <memory>
#include <vector>
#include <filesystem>
#include <fmt/core.h>

namespace seri
{
	class AssetManager
	{
	public:
		AssetManager(AssetManager const&) = delete;

		void operator=(AssetManager const&) = delete;

		AssetManager() = default;

		static AssetManager& GetInstance()
		{
			static AssetManager instance;
			return instance;
		}

		static void Init()
		{
			GetInstance();
		}

		static void Update()
		{
		}

		static void GetWorkingDirectory()
		{
			auto workingDir = std::filesystem::current_path();
			LOGGER(info, fmt::format("[asset] working directory: {}", workingDir.string()));
		}

	private:

	};
}
