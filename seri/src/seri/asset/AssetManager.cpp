#include "Seripch.h"

#include "seri/asset/AssetManager.h"

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
		for (auto& entry : std::filesystem::directory_iterator(path))
		{
			AssetTreeNode child;
			child.id = seri::Random::UUID();
			child.parentId = 0;
			child.isFolder = entry.is_directory();
			child.path = entry.path();
			child.name = entry.path().filename().string();

			if (child.isFolder)
			{
				BuildAssetTree(child, child.path);
			}

			node.children.push_back(std::move(child));
		}
	}
}
