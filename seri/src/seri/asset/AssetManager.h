#pragma once

#include "seri/util/Util.h"
#include "seri/scene/Scene.h"
#include "seri/texture/Skybox.h"
#include "seri/graphic/Material.h"
#include "seri/asset/Assets.h"
#include "seri/asset/AssetBase.h"
#include "seri/asset/AssetWatcher.h"
#include "seri/component/Components.h"
#include "seri/graphic/Model.h"
#include "seri/model/ModelImporter.h"

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
		AssetType type{ AssetType::none };
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

			GetInstance()._assetWatcher = std::make_shared<AssetWatcher>(GetAssetDirectory());
			GetInstance()._fileWatcher = std::make_shared<efsw::FileWatcher>();
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

		static std::vector<seri::asset::AssetMetadata> GetAssetsByType(AssetType type)
		{
			std::vector<seri::asset::AssetMetadata> assets{};
			for (auto& [id, metadata] : GetInstance()._assetMetadataCache)
			{
				if (metadata.type == type)
				{
					assets.push_back(metadata);
				}
			}
			return assets;
		}

		template<typename T>
		static std::shared_ptr<T> GetAssetByID(uint64_t id)
		{
			if (GetInstance()._assetCache.find(id) != GetInstance()._assetCache.end())
			{
				return std::dynamic_pointer_cast<T>(GetInstance()._assetCache[id]);
			}
			return nullptr;
		}

		void Save();
		void InitDefaultAssets();
		void UpdateAssetTree();
		void LoadAfterUpdate();
		void BuildAssetTree(AssetTreeNode& node);

		const char* kAssetFolder = "assets";

		const char* kAssetMetaExtension = "smeta";
		const char* kAssetSceneExtension = "sscene";
		const char* kAssetMaterialExtension = "smat";
		const char* kAssetShaderExtension = "sshader";
		const char* kAssetFBXExtension = "fbx";
		const char* kAssetOBJExtension = "obj";
		const char* kAssetTexturePNGExtension = "png";
		const char* kAssetTextureJPGExtension = "jpg";
		const char* kAssetTextureJPEGExtension = "jpeg";

	private:
		AssetTreeNode _assetTreeRoot{};

		std::unordered_map<uint64_t, std::shared_ptr<AssetBase>> _assetCache{};
		std::unordered_map<uint64_t, seri::asset::AssetMetadata> _assetMetadataCache{};

		std::shared_ptr<AssetWatcher> _assetWatcher;
		std::shared_ptr<efsw::FileWatcher> _fileWatcher;
		efsw::WatchID _watchID{ 0 };

	};
}
