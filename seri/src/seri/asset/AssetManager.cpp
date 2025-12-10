#include "Seripch.h"

#include "seri/asset/AssetManager.h"

#include <yaml-cpp/yaml.h>

namespace seri::asset
{
	void asset::AssetManager::InitDefaultAssets()
	{
	}

	void asset::AssetManager::UpdateAssetTree()
	{
		_assetTreeRoot = {};
		_assetTreeRoot.path = GetAssetDirectory();

		BuildAssetTree(_assetTreeRoot);

		LOGGER(info, "[asset] asset tree updated");
	}

	void asset::AssetManager::BuildAssetTree(AssetTreeNode& parent)
	{
		static bool supportMeta = false;
		static bool deleteAllMeta = true;

		for (auto& entry : std::filesystem::directory_iterator(parent.path))
		{
			AssetTreeNode node;
			node.id = seri::Random::UUID();
			node.isFolder = entry.is_directory();
			node.path = entry.path();
			node.name = entry.path().filename().string();
			node.extension = node.path.extension().string();
			node.meta = fmt::format("{}.smeta", node.path.string());
			node.isMeta = node.extension == ".smeta";

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
							YAML::Node idInfoNode = root["Asset"]["IDInfo"];
							IDInfo idInfo = IDInfo::Deserialize(idInfoNode);

							existingId = idInfo.id;
						}
						else
						{
							existingId = seri::Random::UUID();

							YAML::Node idInfoNode;
							IDInfo idInfo;
							idInfo.id = existingId;
							idInfo.version = "0.1";
							std::ofstream outfile(node.meta);
							idInfoNode["IDInfo"] = IDInfo::Serialize(idInfo);

							YAML::Node root;
							root["Asset"] = idInfoNode;

							outfile << root;
						}
					}
				}

				if (node.extension == ".smeta")
				{
					continue;
				}
				else if (node.extension == ".smat")
				{
					if (_materialCache.find(existingId) == _materialCache.end())
					{
						YAML::Node root = YAML::LoadFile(node.path.string());

						seri::asset::IDInfo idInfoD = seri::asset::IDInfo::Deserialize(root["IDInfo"]);
						std::shared_ptr<Material> material = seri::asset::MaterialAsset::Deserialize(root["Material"]);

						_materialCache[existingId] = material;
					}
				}
				else if (node.extension == ".smesh")
				{
				}
				else if (node.extension == ".sscene")
				{
				}
			}

			parent.children.push_back(std::move(node));
		}
	}

}
