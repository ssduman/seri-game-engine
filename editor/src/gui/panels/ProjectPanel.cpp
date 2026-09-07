#include "gui/panels/ProjectPanel.h"

namespace seri::editor
{
	void ProjectPanel::Draw(GUIContext& ctx)
	{
		seri::asset::AssetTreeNode& assetTreeRoot = seri::asset::AssetManager::GetAssetTreeRoot();

		if (_selectedFolder == nullptr)
		{
			_selectedFolder = &assetTreeRoot;
		}

		ImGui::Columns(2);

		ImGui::BeginChild("Folders", ImVec2(0, 0), true);
		DrawFolderTree(assetTreeRoot);
		ImGui::EndChild();

		ImGui::NextColumn();

		ImGui::BeginChild("Assets", ImVec2(0, 0), true);
		DrawAssetGrid(ctx);
		ImGui::EndChild();

		ImGui::Columns(1);
	}

	void ProjectPanel::DrawFolderTree(seri::asset::AssetTreeNode& node)
	{
		ImGuiTreeNodeFlags flags =
			ImGuiTreeNodeFlags_OpenOnArrow |
			ImGuiTreeNodeFlags_SpanAvailWidth;

		if (node.children.empty())
		{
			flags |= ImGuiTreeNodeFlags_Leaf;
		}
		if (_selectedFolder == &node)
		{
			flags |= ImGuiTreeNodeFlags_Selected;
		}

		std::string label = node.name;
		if (label.empty())
		{
			label = "/assets";
			ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		}

		bool open = ImGui::TreeNodeEx((void*)(intptr_t)node.id, flags, "%s", label.c_str());

		if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
		{
			_selectedFolder = &node;
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

	void ProjectPanel::DrawAssetGrid(GUIContext& ctx)
	{
		if (!_selectedFolder)
		{
			return;
		}

		const float itemSize = 100.0f;
		const float padding = 10.0f;

		int columns = (int)(ImGui::GetContentRegionAvail().x / (itemSize + padding));
		if (columns < 1)
		{
			columns = 1;
		}

		ImGui::Columns(columns, nullptr, false);

		for (const auto& child : _selectedFolder->children)
		{
			if (child.isMeta)
			{
				continue;
			}

			ImGui::BeginGroup();

			ImGui::Button(child.name.c_str(), ImVec2(itemSize, itemSize));

			if (ImGui::IsItemClicked())
			{
				ctx.selectedAsset = child;
				ctx.selectedEntityId = child.id;
				ctx.inspectorType = InspectorType::asset;
			}

			ImGui::EndGroup();
			ImGui::NextColumn();
		}

		ImGui::Columns(1);
	}
}
