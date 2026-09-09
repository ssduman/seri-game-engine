#include "gui/panels/ProjectPanel.h"
#include "gui/EditorWidgets.h"

#include <algorithm>

namespace seri::editor
{
	static ImU32 GetAssetColor(const seri::asset::AssetTreeNode& node)
	{
		if (node.isFolder)
		{
			return IM_COL32(24, 24, 28, 255);
		}

		switch (node.type)
		{
			case seri::asset::AssetType::material:
				return IM_COL32(126, 122, 200, 255);
			case seri::asset::AssetType::shader:
				return IM_COL32(216, 140, 70, 255);
			case seri::asset::AssetType::mesh:
				return IM_COL32(90, 170, 160, 255);
			case seri::asset::AssetType::scene:
				return IM_COL32(110, 170, 110, 255);
			case seri::asset::AssetType::texture:
				return IM_COL32(90, 140, 200, 255);
			default:
				return IM_COL32(120, 120, 126, 255);
		}
	}

	void ProjectPanel::Draw(GUIContext& ctx)
	{
		seri::asset::AssetTreeNode& root = seri::asset::AssetManager::GetAssetTreeRoot();

		uint64_t treeVersion = seri::asset::AssetManager::GetAssetTreeVersion();
		if (_knownTreeVersion != treeVersion)
		{
			_knownTreeVersion = treeVersion;
			_selectionDirty = true;
		}

		seri::asset::AssetTreeNode* folder = _currentFolder.empty() ? nullptr : FindNode(root, _currentFolder);
		if (folder == nullptr)
		{
			folder = &root;
			_currentFolder = root.path;
		}

		SyncSelection(ctx, root);

		DrawToolbar();

		ImGui::Columns(2, "##project", true);

		if (!_columnsInitialized)
		{
			ImGui::SetColumnWidth(0, 220.0f);
			_columnsInitialized = true;
		}

		ImGui::BeginChild("Folders", ImVec2(0, 0), true);
		DrawFolderTree(root);
		ImGui::EndChild();

		ImGui::NextColumn();

		ImGui::BeginChild("Assets", ImVec2(0, 0), true);
		DrawAssetList(ctx, *folder);
		DrawContextMenu();
		ImGui::EndChild();

		ImGui::Columns(1);

		DrawPopups(ctx);

		ApplyPendingActions(ctx);
	}

	void ProjectPanel::DrawToolbar()
	{
		std::filesystem::path root = seri::asset::AssetManager::GetAssetDirectory();

		ImGui::AlignTextToFramePadding();

		if (ImGui::SmallButton("assets"))
		{
			_currentFolder = root;
		}

		std::error_code ec;
		std::filesystem::path relative = std::filesystem::relative(_currentFolder, root, ec);

		if (!ec && !relative.empty() && relative != ".")
		{
			std::filesystem::path accumulated = root;
			int index = 0;

			for (const auto& part : relative)
			{
				accumulated /= part;

				ImGui::SameLine(0.0f, 4.0f);
				ImGui::TextUnformatted(">");
				ImGui::SameLine(0.0f, 4.0f);

				ImGui::PushID(index);
				if (ImGui::SmallButton(part.string().c_str()))
				{
					_currentFolder = accumulated;
				}
				ImGui::PopID();

				index++;
			}
		}

		float searchWidth = 200.0f;
		float spacing = ImGui::GetStyle().ItemSpacing.x;
		float offset = ImGui::GetWindowWidth() - searchWidth - spacing * 2.0f;

		if (offset > ImGui::GetCursorPosX())
		{
			ImGui::SameLine();
			ImGui::SetCursorPosX(offset);

			ImGui::SetNextItemWidth(searchWidth);
			ImGui::InputTextWithHint("##search", "Search...", &_search);
		}

		ImGui::Separator();
	}

	void ProjectPanel::DrawFolderTree(seri::asset::AssetTreeNode& node)
	{
		ImGuiTreeNodeFlags flags =
			ImGuiTreeNodeFlags_OpenOnArrow |
			ImGuiTreeNodeFlags_SpanAvailWidth;

		bool hasSubFolder = false;
		for (const auto& child : node.children)
		{
			if (child.isFolder)
			{
				hasSubFolder = true;
				break;
			}
		}

		if (!hasSubFolder)
		{
			flags |= ImGuiTreeNodeFlags_Leaf;
		}
		if (_currentFolder == node.path)
		{
			flags |= ImGuiTreeNodeFlags_Selected;
		}

		std::string label = node.name;
		if (label.empty())
		{
			label = "assets";
			ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		}

		bool open = ImGui::TreeNodeEx(node.path.string().c_str(), flags, "%s", label.c_str());

		if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
		{
			_currentFolder = node.path;
		}

		if (open)
		{
			for (auto& child : node.children)
			{
				if (child.isFolder)
				{
					DrawFolderTree(child);
				}
			}

			ImGui::TreePop();
		}
	}

	void ProjectPanel::DrawAssetList(GUIContext& ctx, seri::asset::AssetTreeNode& folder)
	{
		std::vector<const seri::asset::AssetTreeNode*> items;

		for (const auto& child : folder.children)
		{
			if (child.isMeta)
			{
				continue;
			}
			if (!_search.empty() && !Util::ContainsIgnoreCase(child.name, _search))
			{
				continue;
			}

			items.push_back(&child);
		}

		std::sort(items.begin(), items.end(),
			[](const seri::asset::AssetTreeNode* lhs, const seri::asset::AssetTreeNode* rhs)
			{
				if (lhs->isFolder != rhs->isFolder)
				{
					return lhs->isFolder;
				}
				return lhs->name < rhs->name;
			}
		);

		if (items.empty())
		{
			ImGui::TextDisabled("%s", _search.empty() ? "folder is empty" : "no match");
			return;
		}

		for (const auto* item : items)
		{
			DrawItem(ctx, *item);
		}
	}

	void ProjectPanel::DrawItem(GUIContext& ctx, const seri::asset::AssetTreeNode& node)
	{
		ImGui::PushID(node.path.string().c_str());

		float rowHeight = ImGui::GetTextLineHeight();
		ImVec2 cursor = ImGui::GetCursorScreenPos();
		float availWidth = ImGui::GetContentRegionAvail().x;

		bool isSelected = _selectedPath == node.path;

		if (ImGui::Selectable("##item", isSelected, ImGuiSelectableFlags_AllowDoubleClick, ImVec2(0.0f, rowHeight)))
		{
			_selectedPath = node.path;
			_selectionDirty = true;
			_focusInspector = true;

			if (node.isFolder && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				_currentFolder = node.path;
			}
		}

		if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
		{
			_selectedPath = node.path;
			_selectionDirty = true;
			_focusInspector = true;
		}

		if (ImGui::BeginPopupContextItem("##item_context"))
		{
			if (ImGui::MenuItem("Rename"))
			{
				_renamePath = node.path;
				_renameBuffer = node.name;
				_openRenamePopup = true;
			}
			if (ImGui::MenuItem("Delete"))
			{
				_deletePath = node.path;
				_openDeletePopup = true;
			}

			ImGui::EndPopup();
		}

		float iconSize = rowHeight - 2.0f;
		ImVec2 iconMin(cursor.x + 2.0f, cursor.y + 1.0f);
		ImVec2 iconMax(iconMin.x + iconSize, iconMin.y + iconSize);

		DrawItemIcon(node, iconMin, iconMax);

		ImVec2 textMin(iconMax.x + 6.0f, cursor.y);
		ImVec2 textMax(cursor.x + availWidth, cursor.y + rowHeight);

		ImGui::RenderTextEllipsis(ImGui::GetWindowDrawList(), textMin, textMax, textMax.x, node.name.c_str(), nullptr, nullptr);

		ImGui::PopID();
	}

	void ProjectPanel::DrawItemIcon(const seri::asset::AssetTreeNode& node, const ImVec2& min, const ImVec2& max)
	{
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		if (node.type == seri::asset::AssetType::texture)
		{
			auto texture = seri::asset::AssetManager::GetAssetByID<seri::TextureBase>(node.id);
			if (texture)
			{
				drawList->AddImage((ImTextureID)(intptr_t)texture->GetHandle(), min, max, ImVec2(0, 1), ImVec2(1, 0));
				return;
			}
		}

		drawList->AddRectFilled(min, max, GetAssetColor(node), 2.0f);

		if (node.isFolder)
		{
			drawList->AddRect(min, max, IM_COL32(110, 110, 118, 255), 2.0f);
		}
	}

	void ProjectPanel::DrawContextMenu()
	{
		if (!ImGui::BeginPopupContextWindow("##project_context", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
		{
			return;
		}

		if (ImGui::BeginMenu("Create"))
		{
			if (ImGui::MenuItem("Folder"))
			{
				_pendingCreateFolder = true;
			}
			if (ImGui::MenuItem("Material"))
			{
				_pendingCreateMaterial = true;
			}

			ImGui::EndMenu();
		}

		ImGui::Separator();

		if (ImGui::MenuItem("Refresh"))
		{
			_pendingRescan = true;
		}

		ImGui::EndPopup();
	}

	void ProjectPanel::DrawPopups(GUIContext& ctx)
	{
		if (_openRenamePopup)
		{
			ImGui::OpenPopup("Rename Asset");
			_openRenamePopup = false;
		}

		if (_openDeletePopup)
		{
			ImGui::OpenPopup("Delete Asset");
			_openDeletePopup = false;
		}

		if (ImGui::BeginPopupModal("Rename Asset", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::SetNextItemWidth(300.0f);

			if (ImGui::IsWindowAppearing())
			{
				ImGui::SetKeyboardFocusHere();
			}

			bool submitted = ImGui::InputText("##name", &_renameBuffer, ImGuiInputTextFlags_EnterReturnsTrue);

			if (ImGui::Button("Rename", ImVec2(120.0f, 0.0f)) || submitted)
			{
				if (seri::asset::AssetManager::RenameAsset(_renamePath, _renameBuffer))
				{
					_selectedPath = _renamePath.parent_path() / _renameBuffer;
					_selectionDirty = true;
				}

				_renamePath.clear();
				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();

			if (ImGui::Button("Cancel", ImVec2(120.0f, 0.0f)))
			{
				_renamePath.clear();
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		if (ImGui::BeginPopupModal("Delete Asset", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("delete '%s' permanently?", _deletePath.filename().string().c_str());
			ImGui::Spacing();

			if (ImGui::Button("Delete", ImVec2(120.0f, 0.0f)))
			{
				if (seri::asset::AssetManager::DeleteAsset(_deletePath))
				{
					if (_selectedPath == _deletePath)
					{
						_selectedPath.clear();
						_selectionDirty = true;
					}
					if (_currentFolder == _deletePath)
					{
						_currentFolder = _deletePath.parent_path();
					}
				}

				_deletePath.clear();
				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();

			if (ImGui::Button("Cancel", ImVec2(120.0f, 0.0f)))
			{
				_deletePath.clear();
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}

	void ProjectPanel::ApplyPendingActions(GUIContext& ctx)
	{
		if (_pendingCreateFolder)
		{
			_pendingCreateFolder = false;

			std::filesystem::path created = seri::asset::AssetManager::CreateFolder(_currentFolder, "new_folder");
			if (!created.empty())
			{
				_selectedPath = created;
				_selectionDirty = true;
			}
		}

		if (_pendingCreateMaterial)
		{
			_pendingCreateMaterial = false;

			uint64_t id = seri::asset::AssetManager::CreateMaterial(_currentFolder, "new_material");
			if (id != 0)
			{
				_selectedPath = seri::asset::AssetManager::GetAssetMetadata(id).source;
				_selectionDirty = true;
			}
		}

		if (_pendingRescan)
		{
			_pendingRescan = false;

			seri::asset::AssetManager::RequestRescan();
		}
	}

	void ProjectPanel::SyncSelection(GUIContext& ctx, seri::asset::AssetTreeNode& root)
	{
		if (!_selectionDirty)
		{
			return;
		}

		_selectionDirty = false;

		seri::asset::AssetTreeNode* selected = _selectedPath.empty() ? nullptr : FindNode(root, _selectedPath);

		if (selected == nullptr)
		{
			_selectedPath.clear();
			_focusInspector = false;

			if (ctx.inspectorType == InspectorType::asset)
			{
				ctx.inspectorType = InspectorType::none;
				ctx.selectedEntityId = 0;
				ctx.selectedAsset = {};
			}

			return;
		}

		if (!_focusInspector && ctx.inspectorType != InspectorType::asset)
		{
			return;
		}

		_focusInspector = false;

		ctx.selectedAsset = *selected;
		ctx.selectedEntityId = selected->id;
		ctx.inspectorType = InspectorType::asset;
	}

	seri::asset::AssetTreeNode* ProjectPanel::FindNode(seri::asset::AssetTreeNode& node, const std::filesystem::path& path)
	{
		if (node.path == path)
		{
			return &node;
		}

		for (auto& child : node.children)
		{
			if (child.path == path)
			{
				return &child;
			}

			if (child.isFolder)
			{
				if (auto* found = FindNode(child, path))
				{
					return found;
				}
			}
		}

		return nullptr;
	}
}
