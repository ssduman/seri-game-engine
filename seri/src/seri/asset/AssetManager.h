#pragma once

#include "seri/util/Util.h"
#include "seri/scene/Scene.h"
#include "seri/texture/Skybox.h"
#include "seri/asset/AssetWatcher.h"
#include "seri/component/Components.h"

#include <entt/entt.hpp>
#include <yaml-cpp/yaml.h>

#include <memory>
#include <vector>
#include <filesystem>
#include <fmt/core.h>

namespace seri::asset
{
	struct AssetTreeNode
	{
		uint64_t id{ 0 };
		uint64_t parentId{ 0 };
		bool isFolder{ false };
		std::filesystem::path path{};
		std::string name{ "" };
		std::vector<AssetTreeNode> children{};
	};

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

			GetInstance()._assetWatcher = std::make_shared<AssetWatcher>(GetAssetDirectory());

			LOGGER(info, fmt::format("[asset] asset directory: {}", GetAssetDirectory().string()));
		}

		static void Update()
		{
		}

		static std::filesystem::path GetWorkingDirectory()
		{
			return std::filesystem::current_path();
		}

		static std::filesystem::path GetAssetDirectory()
		{
			return GetWorkingDirectory() / GetInstance().kAssetFolder;
		}

		static AssetTreeNode& GetAssetTreeRoot()
		{
			return GetInstance()._assetTreeRoot;
		}

		void UpdateAssetTree();
		void BuildAssetTree(AssetTreeNode& node, const std::filesystem::path& path);

		const char* kAssetFolder = "assets";
		const char* kAssetFontFolder = "fonts";
		const char* kAssetShaderFolder = "shaders";
		const char* kAssetSceneFolder = "scenes";

		const char* kAssetSceneExtension = "yaml";

	private:
		AssetTreeNode _assetTreeRoot{};

		std::shared_ptr<AssetWatcher> _assetWatcher;

	};
}
