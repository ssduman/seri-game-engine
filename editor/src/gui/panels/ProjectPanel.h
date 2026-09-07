#pragma once

#include "gui/GUIContext.h"

namespace seri::editor
{
	class ProjectPanel
	{
	public:
		void Draw(GUIContext& ctx);

	private:
		void DrawFolderTree(seri::asset::AssetTreeNode& node);

		void DrawAssetGrid(GUIContext& ctx);

		seri::asset::AssetTreeNode* _selectedFolder{ nullptr };

	};
}
