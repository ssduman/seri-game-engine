#pragma once

#include "gui/GUIContext.h"

namespace seri::editor
{
	class ProjectPanel
	{
	public:
		void Draw(GUIContext& ctx);

	private:
		void DrawToolbar();

		void DrawFolderTree(seri::asset::AssetTreeNode& node);

		void DrawAssetList(GUIContext& ctx, seri::asset::AssetTreeNode& folder);

		void DrawItem(GUIContext& ctx, const seri::asset::AssetTreeNode& node);

		void DrawItemIcon(const seri::asset::AssetTreeNode& node, const ImVec2& min, const ImVec2& max);

		void DrawContextMenu();

		void DrawPopups(GUIContext& ctx);

		void ApplyPendingActions(GUIContext& ctx);

		void SyncSelection(GUIContext& ctx, seri::asset::AssetTreeNode& root);

		seri::asset::AssetTreeNode* FindNode(seri::asset::AssetTreeNode& node, const std::filesystem::path& path);

		std::filesystem::path _currentFolder{};
		std::filesystem::path _selectedPath{};
		std::filesystem::path _renamePath{};
		std::filesystem::path _deletePath{};

		std::string _renameBuffer{};
		std::string _search{};

		uint64_t _knownTreeVersion{ 0 };

		bool _columnsInitialized{ false };
		bool _selectionDirty{ true };
		bool _focusInspector{ false };
		bool _openRenamePopup{ false };
		bool _openDeletePopup{ false };
		bool _pendingCreateFolder{ false };
		bool _pendingCreateMaterial{ false };
		bool _pendingRescan{ false };

	};
}
