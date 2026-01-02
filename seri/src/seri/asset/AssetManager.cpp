#include "Seripch.h"

#include "seri/asset/AssetManager.h"

#include <yaml-cpp/yaml.h>

#include <algorithm>

namespace seri::asset
{
	void asset::AssetManager::Save()
	{
		for (const auto& kv : _assetMetadataCache)
		{
			uint64_t id = kv.first;
			seri::asset::AssetMetadata metadata = kv.second;

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
							LOGGER(error, fmt::format("[asset] could not found material {} to save", id));
							break;
						}

						seri::asset::IDInfo idInfo{
							.id = id,
							.version = "0.1"
						};

						YAML::Node root;
						root["IDInfo"] = seri::asset::IDInfo::Serialize(idInfo);
						root["Material"] = seri::asset::MaterialAsset::Serialize(material);

						std::ofstream fout(metadata.source);
						fout << root;
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
							LOGGER(error, fmt::format("[asset] could not found model {} to save", id));
							break;
						}

						seri::asset::IDInfo idInfo{
							.id = id,
							.version = "0.1"
						};

						YAML::Node root;
						root["IDInfo"] = seri::asset::IDInfo::Serialize(idInfo);
						root["Model"] = seri::asset::MeshAsset::Serialize(model);

						std::ofstream fout(metadata.meta);
						fout << root;
					}
					break;
				default:
					{
						//LOGGER(info, fmt::format("[asset] unexpected asset type for saving: {}", static_cast<int>(metadata.type)));
					}
					break;
			}
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

		BuildAssetTree(_assetTreeRoot);
		LoadAfterUpdate();

		LOGGER(info, "[asset] asset tree updated");
	}

	void asset::AssetManager::BuildAssetTree(AssetTreeNode& parent)
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
				BuildAssetTree(node);
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
							LOGGER(info, fmt::format("[asset] deleting meta '{}' because not has its asset", node.path.string()));
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
							root["IDInfo"] = IDInfo::Serialize(idInfo);;

							std::ofstream outfile(node.meta);
							outfile << root;
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

				node.type = assetMetadata.type;
				_assetMetadataCache[existingId] = assetMetadata;
			}

			parent.children.push_back(std::move(node));
		}
	}

	void asset::AssetManager::LoadAfterUpdate()
	{
		auto GetShader = [&](seri::asset::AssetMetadata metadata) -> std::shared_ptr<AssetBase>
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

		auto GetTexture = [&](seri::asset::AssetMetadata metadata) -> std::shared_ptr<AssetBase>
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

		auto GetMesh = [&](seri::asset::AssetMetadata metadata) -> std::shared_ptr<AssetBase>
			{
				if (metadata.id == 0)
				{
					return nullptr;
				}
				if (_assetCache.find(metadata.id) == _assetCache.end())
				{
					YAML::Node root = YAML::LoadFile(metadata.meta.string());

					std::shared_ptr<seri::Model> model = seri::asset::MeshAsset::Deserialize(root["Model"], metadata);
					model->Build();
					model->id = metadata.id;
					_assetCache[metadata.id] = model;
				}
				return _assetCache[metadata.id];
			};

		for (const auto& kv : _assetMetadataCache)
		{
			uint64_t id = kv.first;
			seri::asset::AssetMetadata metadata = kv.second;

			switch (metadata.type)
			{
				case seri::asset::AssetType::material:
					{
						YAML::Node rootMeta = YAML::LoadFile(metadata.meta.string());
						YAML::Node rootSource = YAML::LoadFile(metadata.source.string());

						seri::asset::IDInfo idInfo = seri::asset::IDInfo::Deserialize(rootMeta["IDInfo"]);

						std::shared_ptr<Material> material;
						if (_assetCache.find(idInfo.id) != _assetCache.end())
						{
							material = std::dynamic_pointer_cast<Material>(_assetCache[idInfo.id]);
						}
						else
						{
							material = seri::asset::MaterialAsset::Deserialize(rootSource["Material"]);
						}
						_assetCache[idInfo.id] = material;

						material->id = idInfo.id;

						if (material->shaderID != 0)
						{
							auto shader = std::dynamic_pointer_cast<ShaderBase>(GetShader(_assetMetadataCache[material->shaderID]));
							material->SetShader(shader->Clone());
							material->shaderID = 0;
						}

						for (auto& textureData : material->textureIDs)
						{
							std::shared_ptr<TextureBase> texture = nullptr;
							if (textureData.id != 0)
							{
								texture = std::dynamic_pointer_cast<TextureBase>(GetTexture(_assetMetadataCache[textureData.id]));
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
					{
						//LOGGER(info, fmt::format("[asset] unexpected asset type for loading: {}", static_cast<int>(metadata.type)));
					}
					break;
			}
		}
	}

}
