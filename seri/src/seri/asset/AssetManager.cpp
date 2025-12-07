#include "Seripch.h"

#include "seri/asset/AssetManager.h"

#include <yaml-cpp/yaml.h>

namespace seri::asset
{
	void asset::AssetManager::UpdateAssetTree()
	{
		_assetTreeRoot = {};

		BuildAssetTree(_assetTreeRoot, GetAssetDirectory());

		LOGGER(info, "[asset] asset tree updated");
	}

	void asset::AssetManager::BuildAssetTree(AssetTreeNode& node, const std::filesystem::path& path)
	{
		static bool deleteAllMeta = false;

		for (auto& entry : std::filesystem::directory_iterator(path))
		{
			AssetTreeNode child;
			child.id = seri::Random::UUID();
			child.parentId = 0;
			child.isFolder = entry.is_directory();
			child.path = entry.path();
			child.name = entry.path().filename().string();
			child.meta = fmt::format("{}.smeta", child.path.string());

			bool isMeta = child.path.extension().string() == ".smeta";
			if (isMeta)
			{
				if (deleteAllMeta)
				{
					LOGGER(info, fmt::format("[asset] deleting meta '{}'", child.path.string()));
					std::filesystem::remove(child.path);
				}
				else
				{
					std::filesystem::path assetOfMeta = child.path;
					assetOfMeta = assetOfMeta.replace_extension("");
					if (!std::filesystem::exists(assetOfMeta))
					{
						LOGGER(info, fmt::format("[asset] deleting meta '{}' because not has its asset", child.path.string()));
						std::filesystem::remove(child.path);
					}
				}

				continue;
			}

			if (!std::filesystem::exists(child.meta))
			{
				YAML::Node idInfoNode;
				IDInfo idinfo;
				idinfo.id = seri::Random::UUID();
				idinfo.version = "0.1";
				std::ofstream outfile(child.meta);
				idInfoNode["IDInfo"] = IDInfo::Serialize(idinfo);

				YAML::Node root;
				root["Asset"] = idInfoNode;

				outfile << root;
			}

			if (child.isFolder)
			{
				BuildAssetTree(child, child.path);
			}

			node.children.push_back(std::move(child));
		}
	}
}
