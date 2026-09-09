#include "Seripch.h"

#include "seri/asset/AssetManager.h"

#include <yaml-cpp/yaml.h>

#include <algorithm>

namespace seri::asset
{
	static std::string GetPathKey(const std::filesystem::path& path)
	{
		std::error_code ec;
		std::filesystem::path absolute = std::filesystem::absolute(path, ec);
		if (ec)
		{
			absolute = path;
		}

		return absolute.lexically_normal().string();
	}

	static FileState GetFileState(const std::filesystem::path& path)
	{
		FileState state;

		std::error_code ec;
		std::filesystem::file_status status = std::filesystem::status(path, ec);
		if (ec || !std::filesystem::exists(status))
		{
			return state;
		}

		state.exists = true;
		state.isDirectory = std::filesystem::is_directory(status);

		if (state.isDirectory)
		{
			return state;
		}

		state.size = std::filesystem::file_size(path, ec);
		if (ec)
		{
			return {};
		}

		state.time = std::filesystem::last_write_time(path, ec);
		if (ec)
		{
			return {};
		}

		return state;
	}

	void asset::AssetManager::Save()
	{
		for (const auto& kv : _assetMetadataCache)
		{
			SaveAssetInternal(kv.first, kv.second);
		}
	}

	bool asset::AssetManager::SaveAsset(uint64_t id)
	{
		AssetManager& instance = GetInstance();

		if (instance._assetMetadataCache.find(id) == instance._assetMetadataCache.end())
		{
			return false;
		}

		instance.SaveAssetInternal(id, instance._assetMetadataCache[id]);

		return true;
	}

	void asset::AssetManager::SaveAssetInternal(uint64_t id, const seri::asset::AssetMetadata& metadata)
	{
		switch (metadata.type)
		{
			case seri::asset::AssetType::material:
				{
					std::shared_ptr<Material> material;
					if (_assetCache.find(id) != _assetCache.end())
					{
						material = std::dynamic_pointer_cast<Material>(_assetCache[id]);
					}
					else
					{
						LIB_LOGGER(error, asset) << fmt::format("could not found material {} to save", id);
						break;
					}

					seri::asset::IDInfo idInfo{
						.id = id,
						.version = "0.1"
					};

					YAML::Node root;
					root["IDInfo"] = seri::asset::IDInfo::Serialize(idInfo);
					root["Material"] = seri::asset::MaterialAsset::Serialize(material);

					WriteAssetFile(metadata.source, root);
				}
				break;
			case seri::asset::AssetType::shader:
				{
				}
				break;
			case seri::asset::AssetType::texture:
				{
				}
				break;
			case seri::asset::AssetType::mesh:
				{
					std::shared_ptr<Model> model;
					if (_assetCache.find(id) != _assetCache.end())
					{
						model = std::dynamic_pointer_cast<Model>(_assetCache[id]);
					}
					else
					{
						LIB_LOGGER(error, asset) << fmt::format("could not found model {} to save", id);
						break;
					}

					seri::asset::IDInfo idInfo{
						.id = id,
						.version = "0.1"
					};

					YAML::Node root;
					root["IDInfo"] = seri::asset::IDInfo::Serialize(idInfo);
					root["Model"] = seri::asset::MeshAsset::Serialize(model);

					WriteAssetFile(metadata.meta, root);
				}
				break;
			default:
				break;
		}
	}

	void asset::AssetManager::InitDefaultAssets()
	{
	}

	void asset::AssetManager::UpdateAssetTree()
	{
		_assetMetadataCache = {};

		_assetTreeRoot = {};
		_assetTreeRoot.path = GetAssetDirectory();

		std::unordered_map<std::string, FileState> scanStates;
		scanStates[GetPathKey(_assetTreeRoot.path)] = GetFileState(_assetTreeRoot.path);

		BuildAssetTree(_assetTreeRoot, scanStates);
		LoadAfterUpdate();

		{
			std::lock_guard<std::mutex> lock(_watcherMutex);
			_knownStates = std::move(scanStates);
		}

		_assetTreeVersion++;

		LIB_LOGGER(info, asset) << "asset tree updated";
	}

	void asset::AssetManager::BuildAssetTree(AssetTreeNode& parent, std::unordered_map<std::string, FileState>& states)
	{
		static bool supportMeta = true;
		static bool deleteAllMeta = false;

		for (auto& entry : std::filesystem::directory_iterator(parent.path))
		{
			std::string extension = entry.path().extension().string();
			if (!extension.empty() && extension[0] == '.')
			{
				extension.erase(0, 1);
			}
			seri::Util::ToLower(extension);

			states[GetPathKey(entry.path())] = GetFileState(entry.path());

			AssetTreeNode node;
			node.id = seri::Random::UUID();
			node.isFolder = entry.is_directory();
			node.path = entry.path();
			node.name = entry.path().filename().string();
			node.extension = extension;
			node.meta = fmt::format("{}.{}", node.path.string(), kAssetMetaExtension);
			node.isMeta = node.extension == kAssetMetaExtension;

			if (node.isFolder)
			{
				BuildAssetTree(node, states);
			}
			else
			{
				uint64_t existingId = 0;
				if (supportMeta)
				{
					if (node.isMeta)
					{
						std::filesystem::path assetOfMeta = node.path;
						assetOfMeta = assetOfMeta.replace_extension("");
						if (deleteAllMeta || !std::filesystem::exists(assetOfMeta))
						{
							LIB_LOGGER(info, asset) << fmt::format("deleting meta '{}' because not has its asset", node.path.string());
							std::filesystem::remove(node.path);
						}
					}
					else
					{
						if (std::filesystem::exists(node.meta))
						{
							YAML::Node root = YAML::LoadFile(node.meta.string());
							YAML::Node idInfoNode = root["IDInfo"];
							IDInfo idInfo = IDInfo::Deserialize(idInfoNode);

							existingId = idInfo.id;
						}
						else
						{
							existingId = seri::Random::UUID();

							IDInfo idInfo;
							idInfo.id = existingId;
							idInfo.version = "0.1";

							YAML::Node root;
							root["IDInfo"] = IDInfo::Serialize(idInfo);

							WriteAssetFile(node.meta, root);
						}
					}
				}

				if (node.isMeta)
				{
					continue;
				}

				seri::asset::AssetMetadata assetMetadata = seri::asset::AssetMetadata{
					.id = existingId,
					.type = seri::asset::AssetType::none,
					.name = node.name,
					.source = node.path,
					.meta = node.meta,
				};
				node.id = existingId;

				if (node.extension == kAssetMetaExtension)
				{
				}
				else if (node.extension == kAssetMaterialExtension)
				{
					assetMetadata.type = seri::asset::AssetType::material;
				}
				else if (node.extension == kAssetShaderExtension)
				{
					assetMetadata.type = seri::asset::AssetType::shader;
				}
				else if (node.extension == kAssetSceneExtension)
				{
					assetMetadata.type = seri::asset::AssetType::scene;
				}
				else if (node.extension == kAssetFBXExtension)
				{
					assetMetadata.type = seri::asset::AssetType::mesh;
				}
				else if (node.extension == kAssetOBJExtension)
				{
					assetMetadata.type = seri::asset::AssetType::mesh;
				}
				else if (node.extension == kAssetTexturePNGExtension)
				{
					assetMetadata.type = seri::asset::AssetType::texture;
				}
				else if (node.extension == kAssetTextureJPGExtension)
				{
					assetMetadata.type = seri::asset::AssetType::texture;
				}
				else if (node.extension == kAssetTextureJPEGExtension)
				{
					assetMetadata.type = seri::asset::AssetType::texture;
				}
				else if (node.extension == kAssetTextureTGAExtension)
				{
					assetMetadata.type = seri::asset::AssetType::texture;
				}

				node.type = assetMetadata.type;
				_assetMetadataCache[existingId] = assetMetadata;
			}

			parent.children.push_back(std::move(node));
		}
	}

	void asset::AssetManager::LoadAfterUpdate()
	{
		auto GetShader = [&](const seri::asset::AssetMetadata& metadata) -> std::shared_ptr<AssetBase>
			{
				if (metadata.id == 0)
				{
					return nullptr;
				}
				if (_assetCache.find(metadata.id) == _assetCache.end())
				{
					std::shared_ptr<seri::ShaderBase> shader = ShaderBase::Create();
					shader->Init(metadata.source.string());
					shader->id = metadata.id;
					_assetCache[metadata.id] = shader;
				}
				return _assetCache[metadata.id];
			};

		auto GetTexture = [&](const seri::asset::AssetMetadata& metadata) -> std::shared_ptr<AssetBase>
			{
				if (metadata.id == 0)
				{
					return nullptr;
				}
				if (_assetCache.find(metadata.id) == _assetCache.end())
				{
					std::shared_ptr<seri::TextureBase> texture = seri::TextureBase::Create();
					texture->Init(seri::TextureDesc{}, metadata.source.string());
					texture->id = metadata.id;
					_assetCache[metadata.id] = texture;
				}
				return _assetCache[metadata.id];
			};

		auto GetMesh = [&](const seri::asset::AssetMetadata& metadata) -> std::shared_ptr<AssetBase>
			{
				if (metadata.id == 0)
				{
					return nullptr;
				}
				if (_assetCache.find(metadata.id) == _assetCache.end())
				{
					YAML::Node root{};
					if (std::filesystem::exists(metadata.meta))
					{
						root = YAML::LoadFile(metadata.meta.string());
					}

					std::shared_ptr<seri::Model> model = seri::asset::MeshAsset::Deserialize(root["Model"], metadata);
					if (!model)
					{
						return nullptr;
					}

					model->Build();
					model->id = metadata.id;
					_assetCache[metadata.id] = model;
				}
				return _assetCache[metadata.id];
			};

		auto FindMetadata = [&](uint64_t id) -> seri::asset::AssetMetadata
			{
				auto it = _assetMetadataCache.find(id);
				if (it == _assetMetadataCache.end())
				{
					return {};
				}
				return it->second;
			};

		std::vector<seri::asset::AssetMetadata> metadataList;
		metadataList.reserve(_assetMetadataCache.size());
		for (const auto& kv : _assetMetadataCache)
		{
			metadataList.push_back(kv.second);
		}

		for (const auto& metadata : metadataList)
		{
			try
			{
				switch (metadata.type)
				{
					case seri::asset::AssetType::material:
						{
							YAML::Node rootSource = YAML::LoadFile(metadata.source.string());

							std::shared_ptr<Material> material;
							if (_assetCache.find(metadata.id) != _assetCache.end())
							{
								material = std::dynamic_pointer_cast<Material>(_assetCache[metadata.id]);
							}
							else
							{
								material = seri::asset::MaterialAsset::Deserialize(rootSource["Material"]);
							}
							_assetCache[metadata.id] = material;

							material->id = metadata.id;

							if (material->shaderID != 0)
							{
								auto shader = std::dynamic_pointer_cast<ShaderBase>(GetShader(FindMetadata(material->shaderID)));
								if (shader)
								{
									material->SetShader(shader->Clone());
								}
								else
								{
									LIB_LOGGER(error, asset) << fmt::format("shader {} of material {} not found", material->shaderID, metadata.name);
								}
								material->shaderID = 0;
							}

							for (auto& textureData : material->textureIDs)
							{
								std::shared_ptr<TextureBase> texture = nullptr;
								if (textureData.id != 0)
								{
									texture = std::dynamic_pointer_cast<TextureBase>(GetTexture(FindMetadata(textureData.id)));
								}
								material->SetTexture(textureData.name, texture);
							}
							material->textureIDs = {};
						}
						break;
					case seri::asset::AssetType::shader:
						{
							GetShader(metadata);
						}
						break;
					case seri::asset::AssetType::texture:
						{
							GetTexture(metadata);
						}
						break;
					case seri::asset::AssetType::mesh:
						{
							GetMesh(metadata);
						}
						break;
					default:
						break;
				}
			}
			catch (const std::exception& e)
			{
				LIB_LOGGER(error, asset) << fmt::format("loading asset {} failed: {}", metadata.source.string(), e.what());
			}
		}
	}

	void asset::AssetManager::Update()
	{
		AssetManager& instance = GetInstance();

		bool rescan = false;

		{
			std::lock_guard<std::mutex> lock(instance._watcherMutex);

			if (instance._rescanPending)
			{
				std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
				if (now - instance._lastEventTime >= std::chrono::milliseconds(kRescanDelayMs))
				{
					if (instance.IsPendingStable())
					{
						instance._rescanPending = false;
						rescan = instance.HasPendingChanges();
						instance._pendingFiles.clear();
					}
					else
					{
						instance._lastEventTime = now;
					}
				}
			}
		}

		if (rescan)
		{
			instance.UpdateAssetTree();
		}
	}

	void asset::AssetManager::NotifyFileEvent(const std::filesystem::path& path)
	{
		AssetManager& instance = GetInstance();

		std::string key = GetPathKey(path);
		FileState state = GetFileState(path);

		std::lock_guard<std::mutex> lock(instance._watcherMutex);

		auto it = instance._knownStates.find(key);
		bool known = it != instance._knownStates.end();

		if (!state.exists)
		{
			if (!known)
			{
				return;
			}
		}
		else if (state.isDirectory)
		{
			if (known && it->second.isDirectory)
			{
				return;
			}
		}
		else if (known && it->second == state)
		{
			return;
		}

		instance._pendingFiles[key] = state;
		instance._rescanPending = true;
		instance._lastEventTime = std::chrono::steady_clock::now();
	}

	void asset::AssetManager::RequestRescan()
	{
		GetInstance().UpdateAssetTree();
	}

	bool asset::AssetManager::IsPendingStable()
	{
		bool stable = true;

		for (auto& kv : _pendingFiles)
		{
			FileState state = GetFileState(kv.first);

			if (state.exists && !state.isDirectory)
			{
				if (!(state == kv.second))
				{
					stable = false;
				}
				else
				{
					std::ifstream file(kv.first, std::ios::binary);
					if (!file.is_open())
					{
						stable = false;
					}
				}
			}

			kv.second = state;
		}

		return stable;
	}

	bool asset::AssetManager::HasPendingChanges()
	{
		for (const auto& kv : _pendingFiles)
		{
			auto it = _knownStates.find(kv.first);
			bool known = it != _knownStates.end();

			if (!kv.second.exists)
			{
				if (known)
				{
					return true;
				}
				continue;
			}

			if (!known)
			{
				return true;
			}

			if (kv.second.isDirectory)
			{
				continue;
			}

			if (!(kv.second == it->second))
			{
				return true;
			}
		}

		return false;
	}

	void asset::AssetManager::WriteAssetFile(const std::filesystem::path& path, const YAML::Node& root)
	{
		{
			std::ofstream fout(path);
			if (!fout.is_open())
			{
				LIB_LOGGER(error, asset) << fmt::format("could not write {}", path.string());
				return;
			}

			fout << root;
		}

		GetInstance().RememberPath(path);
	}

	void asset::AssetManager::RememberPath(const std::filesystem::path& path)
	{
		std::string key = GetPathKey(path);
		FileState state = GetFileState(path);

		std::lock_guard<std::mutex> lock(_watcherMutex);

		if (!state.exists)
		{
			_knownStates.erase(key);
			return;
		}

		_knownStates[key] = state;
	}

	void asset::AssetManager::ForgetPath(const std::filesystem::path& path)
	{
		std::lock_guard<std::mutex> lock(_watcherMutex);

		_knownStates.erase(GetPathKey(path));
	}

	uint64_t asset::AssetManager::CreateMaterial(const std::filesystem::path& folder, const std::string& name)
	{
		AssetManager& instance = GetInstance();

		std::filesystem::path source = instance.GetUniquePath(folder, name, instance.kAssetMaterialExtension);
		std::filesystem::path meta = instance.GetMetaPath(source);

		std::shared_ptr<Material> material = std::make_shared<Material>();
		material->id = seri::Random::UUID();

		uint64_t shaderId = instance.FindAssetIdByName(instance.kDefaultShaderName);
		if (shaderId != 0)
		{
			std::shared_ptr<ShaderBase> shader = GetAssetByID<ShaderBase>(shaderId);
			if (shader)
			{
				material->SetShader(shader->Clone());
			}
		}

		seri::asset::IDInfo idInfo{
			.id = material->id,
			.version = "0.1"
		};

		YAML::Node sourceRoot;
		sourceRoot["IDInfo"] = seri::asset::IDInfo::Serialize(idInfo);
		sourceRoot["Material"] = seri::asset::MaterialAsset::Serialize(material);

		YAML::Node metaRoot;
		metaRoot["IDInfo"] = seri::asset::IDInfo::Serialize(idInfo);

		instance.WriteAssetFile(source, sourceRoot);
		instance.WriteAssetFile(meta, metaRoot);

		instance._assetCache[material->id] = material;

		instance.UpdateAssetTree();

		LIB_LOGGER(info, asset) << fmt::format("material created: {}", source.string());

		return material->id;
	}

	std::filesystem::path asset::AssetManager::CreateFolder(const std::filesystem::path& folder, const std::string& name)
	{
		AssetManager& instance = GetInstance();

		std::filesystem::path target = folder / name;

		int index = 1;
		while (std::filesystem::exists(target))
		{
			target = folder / fmt::format("{} {}", name, index);
			index++;
		}

		std::error_code ec;
		if (!std::filesystem::create_directory(target, ec))
		{
			LIB_LOGGER(error, asset) << fmt::format("could not create folder {}: {}", target.string(), ec.message());
			return {};
		}

		instance.RememberPath(target);

		instance.UpdateAssetTree();

		return target;
	}

	bool asset::AssetManager::RenameAsset(const std::filesystem::path& path, const std::string& newName)
	{
		AssetManager& instance = GetInstance();

		if (newName.empty() || path.filename().string() == newName)
		{
			return false;
		}

		std::filesystem::path target = path.parent_path() / newName;
		if (std::filesystem::exists(target))
		{
			LIB_LOGGER(error, asset) << fmt::format("already exists: {}", target.string());
			return false;
		}

		std::error_code ec;
		std::filesystem::rename(path, target, ec);
		if (ec)
		{
			LIB_LOGGER(error, asset) << fmt::format("could not rename {}: {}", path.string(), ec.message());
			return false;
		}

		instance.ForgetPath(path);
		instance.RememberPath(target);

		std::filesystem::path meta = instance.GetMetaPath(path);
		if (std::filesystem::exists(meta))
		{
			std::filesystem::rename(meta, instance.GetMetaPath(target), ec);
		}

		instance.UpdateAssetTree();

		return true;
	}

	bool asset::AssetManager::DeleteAsset(const std::filesystem::path& path)
	{
		AssetManager& instance = GetInstance();

		std::error_code ec;
		if (!std::filesystem::exists(path, ec))
		{
			return false;
		}

		uint64_t id = instance.FindAssetIdByPath(path);

		if (std::filesystem::is_directory(path, ec))
		{
			std::filesystem::remove_all(path, ec);
		}
		else
		{
			std::filesystem::remove(path, ec);
			std::filesystem::remove(instance.GetMetaPath(path), ec);
		}

		if (ec)
		{
			LIB_LOGGER(error, asset) << fmt::format("could not delete {}: {}", path.string(), ec.message());
			return false;
		}

		instance.ForgetPath(path);

		if (id != 0)
		{
			instance._assetCache.erase(id);
		}

		instance.UpdateAssetTree();

		return true;
	}

	uint64_t asset::AssetManager::FindAssetIdByName(std::string_view name)
	{
		for (const auto& kv : _assetMetadataCache)
		{
			if (kv.second.name == name)
			{
				return kv.first;
			}
		}

		return 0;
	}

	uint64_t asset::AssetManager::FindAssetIdByPath(const std::filesystem::path& path)
	{
		for (const auto& kv : _assetMetadataCache)
		{
			if (kv.second.source == path)
			{
				return kv.first;
			}
		}

		return 0;
	}

	std::filesystem::path asset::AssetManager::GetUniquePath(const std::filesystem::path& folder, const std::string& name, const char* extension)
	{
		std::filesystem::path target = folder / fmt::format("{}.{}", name, extension);

		int index = 1;
		while (std::filesystem::exists(target))
		{
			target = folder / fmt::format("{} {}.{}", name, index, extension);
			index++;
		}

		return target;
	}

	std::filesystem::path asset::AssetManager::GetMetaPath(const std::filesystem::path& path)
	{
		return fmt::format("{}.{}", path.string(), kAssetMetaExtension);
	}

}
