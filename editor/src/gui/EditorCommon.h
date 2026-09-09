#pragma once

#include <seri/core/Core.h>
#include <seri/core/Seri.h>
#include <seri/util/Util.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>

#include <ImGuizmo.h>

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

namespace seri::editor
{
	struct ScopedChild
	{
		ScopedChild(const char* id, ImVec2 size = ImVec2(0, 0), ImGuiChildFlags flags = ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY)
		{
			ImGui::BeginChild(id, size, flags);
		}

		~ScopedChild()
		{
			ImGui::EndChild();
		}
	};

	ImVec4 RGBNormalized(int r, int g, int b);

	void Save();

	const char* GetAssetTypeName(const seri::asset::AssetTreeNode& node);
}
