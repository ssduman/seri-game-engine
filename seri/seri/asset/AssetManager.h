#pragma once

#include "seri/core/Singleton.h"

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
#include "seri/shader/ShaderBase.h"

#include <entt/entt.hpp>
#include <efsw/efsw.hpp>
#include <yaml-cpp/yaml.h>

#include <mutex>
#include <chrono>
#include <memory>
#include <vector>
#include <filesystem>
#include <fmt/format.h>

namespace seri::asset
{
	struct FileState
	{
		bool exists{ false };
		bool isDirectory{ false };
		uintmax_t size{ 0 };
		std::filesystem::file_time_type time{};

		bool operator==(const FileState& other) const = default;
	};

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

	class AssetManager : public seri::Singleton<AssetManager>
	{
	public:
		static void Init()
		{
			GetInstance();

			GetInstance().UpdateAssetTree();

			GetInstance()._assetWatcher = std::make_shared<AssetWatcher>(GetAssetDirectory());
			GetInstance()._fileWatcher = std::make_shared<efsw::FileWatcher>();
			GetInstance()._fileWatcher->watch();

			LIB_LOGGER(info, asset) << fmt::format("asset directory: {}", GetAssetDirectory().string());
		}

		static void Update();

		static void NotifyFileEvent(const std::filesystem::path& path);

		static void RequestRescan();

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

		static uint64_t GetAssetTreeVersion()
		{
			return GetInstance()._assetTreeVersion;
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

		static std::string GetAssetName(uint64_t id)
		{
			if (GetInstance()._assetMetadataCache.find(id) != GetInstance()._assetMetadataCache.end())
			{
				return GetInstance()._assetMetadataCache[id].name;
			}
			return "<not_found>";
		}

		static seri::asset::AssetMetadata GetAssetMetadata(uint64_t id)
		{
			if (GetInstance()._assetMetadataCache.find(id) != GetInstance()._assetMetadataCache.end())
			{
				return GetInstance()._assetMetadataCache[id];
			}
			return {};
		}

		static uint64_t CreateMaterial(const std::filesystem::path& folder, const std::string& name);

		static std::filesystem::path CreateFolder(const std::filesystem::path& folder, const std::string& name);

		static bool RenameAsset(const std::filesystem::path& path, const std::string& newName);

		static bool DeleteAsset(const std::filesystem::path& path);

		static bool SaveAsset(uint64_t id);

		static void WriteAssetFile(const std::filesystem::path& path, const YAML::Node& root);

		template<typename T>
		static std::shared_ptr<T> GetAssetByID(uint64_t id)
		{
			if (GetInstance()._assetCache.find(id) != GetInstance()._assetCache.end())
			{
				return std::dynamic_pointer_cast<T>(GetInstance()._assetCache[id]);
			}
			return nullptr;
		}

		static void AddAsset(uint64_t id, std::shared_ptr<AssetBase> asset)
		{
			asset->id = id;
			GetInstance()._assetCache[id] = asset;
		}

		void Save();
		void InitDefaultAssets();
		void UpdateAssetTree();
		void LoadAfterUpdate();
		void BuildAssetTree(AssetTreeNode& node, std::unordered_map<std::string, FileState>& states);

		const char* kAssetFolder = "assets";

		const char* kDefaultShaderName = "pbr.sshader";

		static const int kRescanDelayMs = 400;

		const char* kAssetMetaExtension = "smeta";
		const char* kAssetSceneExtension = "sscene";
		const char* kAssetMaterialExtension = "smat";
		const char* kAssetShaderExtension = "sshader";
		const char* kAssetFBXExtension = "fbx";
		const char* kAssetOBJExtension = "obj";
		const char* kAssetTexturePNGExtension = "png";
		const char* kAssetTextureJPGExtension = "jpg";
		const char* kAssetTextureJPEGExtension = "jpeg";
		const char* kAssetTextureTGAExtension = "tga";
		const char* kAssetFontTTFExtension = "ttf";
		const char* kAssetFontOTFExtension = "otf";

	protected:
		friend struct seri::Singleton<AssetManager>;

		AssetManager() = default;
		~AssetManager() = default;

	private:
		void SaveAssetInternal(uint64_t id, const seri::asset::AssetMetadata& metadata);
		void RememberPath(const std::filesystem::path& path);
		void ForgetPath(const std::filesystem::path& path);
		bool IsPendingStable();
		bool HasPendingChanges();
		uint64_t FindAssetIdByName(std::string_view name);
		uint64_t FindAssetIdByPath(const std::filesystem::path& path);
		std::filesystem::path GetUniquePath(const std::filesystem::path& folder, const std::string& name, const char* extension);
		std::filesystem::path GetMetaPath(const std::filesystem::path& path);

		AssetTreeNode _assetTreeRoot{};
		uint64_t _assetTreeVersion{ 0 };

		std::unordered_map<uint64_t, std::shared_ptr<AssetBase>> _assetCache{};
		std::unordered_map<uint64_t, seri::asset::AssetMetadata> _assetMetadataCache{};

		std::shared_ptr<AssetWatcher> _assetWatcher;
		std::shared_ptr<efsw::FileWatcher> _fileWatcher;
		efsw::WatchID _watchID{ 0 };

		std::mutex _watcherMutex{};
		bool _rescanPending{ false };
		std::chrono::steady_clock::time_point _lastEventTime{};
		std::unordered_map<std::string, FileState> _knownStates{};
		std::unordered_map<std::string, FileState> _pendingFiles{};

	};
}
