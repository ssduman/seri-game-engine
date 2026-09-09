#include "gui/EditorCommon.h"

namespace seri::editor
{
	ImVec4 RGBNormalized(int r, int g, int b)
	{
		return ImVec4(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
	}

	void Save()
	{
		if (auto activeScene = seri::scene::SceneManager::GetActiveScene())
		{
			activeScene->Save();
		}

		seri::asset::AssetManager::GetInstance().Save();
	}

	const char* GetAssetTypeName(const seri::asset::AssetTreeNode& node)
	{
		if (node.isFolder)
		{
			return "folder";
		}

		if (node.type == seri::asset::AssetType::none)
		{
			return "file";
		}

		return seri::asset::AssetTypeToString(node.type);
	}
}
