#pragma once

#include "seri/util/Util.h"
#include "seri/scene/Scene.h"
#include "seri/texture/Skybox.h"
#include "seri/graphic/Material.h"
#include "seri/asset/Assets.h"
#include "seri/asset/AssetWatcher.h"
#include "seri/component/Components.h"

#include <entt/entt.hpp>
#include <efsw/efsw.hpp>
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
		bool isFolder{ false };
		bool isMeta{ false };
		std::filesystem::path path{};
		std::filesystem::path meta{};
		std::string name{ "" };
		std::string extension{ "" };
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

			GetInstance().UpdateAssetTree();

			GetInstance()._fileWatcher = std::make_shared<efsw::FileWatcher>();
			GetInstance()._assetWatcher = std::make_shared<AssetWatcher>(GetAssetDirectory());
			GetInstance()._fileWatcher->watch();

			LOGGER(info, fmt::format("[asset] asset directory: {}", GetAssetDirectory().string()));
		}

		static void Update()
		{
		}

		static void StartAssetWatcher()
		{
			GetInstance()._watchID = GetInstance()._fileWatcher->addWatch(GetAssetDirectory().string(), GetInstance()._assetWatcher.get(), /*recursive*/ true);
		}

		static void StopAssetWatcher()
		{
			if (GetInstance()._watchID == 0)
			{
				return;
			}

			GetInstance()._fileWatcher->removeWatch(GetInstance()._watchID);
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

		void InitDefaultAssets();
		void UpdateAssetTree();
		void BuildAssetTree(AssetTreeNode& node);

		const char* kAssetFolder = "assets";
		const char* kAssetFontFolder = "fonts";
		const char* kAssetShaderFolder = "shaders";
		const char* kAssetSceneFolder = "scenes";

		const char* kAssetSceneExtension = "yaml";

	private:
		AssetTreeNode _assetTreeRoot{};

		std::unordered_map<uint64_t, std::filesystem::path> _pathCache{};
		std::unordered_map<uint64_t, std::shared_ptr<Material>> _materialCache{};

		std::shared_ptr<efsw::FileWatcher> _fileWatcher;
		std::shared_ptr<AssetWatcher> _assetWatcher;
		efsw::WatchID _watchID{ 0 };

	};
}
