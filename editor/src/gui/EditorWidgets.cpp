#include "gui/EditorWidgets.h"

namespace seri::editor
{
	bool ShowEditorScriptPickerPopup(std::string& selection)
	{
		static char search[128] = "";

		selection.clear();

		ImGui::SetNextWindowSizeConstraints(ImVec2(280.0f, 0.0f), ImVec2(FLT_MAX, 400.0f));

		if (!ImGui::BeginPopup("ScriptPickerPopup"))
		{
			return false;
		}

		ImGui::TextUnformatted("Add Script");
		ImGui::Separator();

		if (ImGui::IsWindowAppearing())
		{
			search[0] = '\0';
			ImGui::SetKeyboardFocusHere();
		}

		ImGui::SetNextItemWidth(-1);
		ImGui::InputTextWithHint("##Search", "Search...", search, IM_ARRAYSIZE(search));

		ImGui::Spacing();

		bool picked = false;

		for (const auto& name : seri::script::ScriptRegistry::GetNames())
		{
			if (!Util::ContainsIgnoreCase(name, search))
			{
				continue;
			}

			ImGui::PushID(name.c_str());

			if (ImGui::Selectable(name.c_str()))
			{
				selection = name;
				picked = true;
				ImGui::CloseCurrentPopup();
			}

			ImGui::PopID();

			if (picked)
			{
				break;
			}
		}

		ImGui::EndPopup();

		return picked;
	}

	bool ShowEditorAssetPickerPopup(seri::asset::AssetType type, bool& selected, uint64_t& selection)
	{
		selected = false;
		selection = 0;

		ImGui::SetNextWindowSizeConstraints(ImVec2(320.0f, 0.0f), ImVec2(FLT_MAX, 400.0f));

		if (!ImGui::BeginPopup("AssetPickerPopup"))
		{
			return false;
		}

		static char search[64]{};
		const float size = 64.0f;

		if (ImGui::IsWindowAppearing())
		{
			search[0] = '\0';
			ImGui::SetKeyboardFocusHere();
		}

		ImGui::SetNextItemWidth(-1);
		ImGui::InputTextWithHint("##Search", "Search...", search, sizeof(search));
		ImGui::Separator();

		switch (type)
		{
			case seri::asset::AssetType::texture:
				{
					int columnCount = int(ImGui::GetContentRegionAvail().x / size);
					ImGui::Columns(columnCount > 0 ? columnCount : 1);
				}
				break;
		}

		seri::asset::AssetMetadata noneAsset{
			.id = 0,
			.name = "<none>"
		};
		auto assetMedataList = seri::asset::AssetManager::GetAssetsByType(type);
		assetMedataList.emplace_back(noneAsset);

		for (auto assetMetadata : assetMedataList)
		{
			if (!Util::ContainsIgnoreCase(assetMetadata.name, search))
			{
				continue;
			}

			ImGui::PushID((void*)(intptr_t)assetMetadata.id);

			if (assetMetadata.id == 0)
			{
				assetMetadata.type = seri::asset::AssetType::none;
			}

			switch (assetMetadata.type)
			{
				case seri::asset::AssetType::texture:
					{
						auto asset = seri::asset::AssetManager::GetAssetByID<seri::TextureBase>(assetMetadata.id);

						if (ShowEditorImageButton(asset, size))
						{
							selected = true;
							selection = assetMetadata.id;
							ImGui::CloseCurrentPopup();
							ImGui::PopID();
							ImGui::EndPopup();
							return true;
						}

						ImGui::TextWrapped("%s", assetMetadata.name.c_str());
						ImGui::NextColumn();
					}
					break;
				default:
					{
						if (ImGui::Selectable(assetMetadata.name.c_str()))
						{
							selected = true;
							selection = assetMetadata.id;
							ImGui::CloseCurrentPopup();
							ImGui::PopID();
							ImGui::EndPopup();
							return true;
						}
					}
					break;
			}

			ImGui::PopID();
		}

		switch (type)
		{
			case seri::asset::AssetType::texture:
				{
					ImGui::Columns(1);
				}
				break;
		}

		ImGui::EndPopup();
		return false;
	}

	void ShowEditorImage(std::shared_ptr<seri::TextureBase>& texture, float size, bool flip)
	{
		if (!texture)
		{
			ImGui::Dummy({ size, size });
			return;
		}

		auto tex = (ImTextureID)(intptr_t)texture->GetHandle();
		ImVec2 uv0 = flip ? ImVec2{ 0, 1 } : ImVec2{ 0, 0 };
		ImVec2 uv1 = flip ? ImVec2{ 1, 0 } : ImVec2{ 1, 1 };
		ImGui::Image(tex, { size, size }, uv0, uv1);
	}

	bool ShowEditorImageButton(std::shared_ptr<seri::TextureBase>& texture, float size)
	{
		if (!texture)
		{
			return ImGui::Button("##missing", { size, size });
		}

		auto tex = (ImTextureID)(intptr_t)texture->GetHandle();
		return ImGui::ImageButton(fmt::format("tex##{}", texture->id).c_str(), tex, { size, size }, { 0, 1 }, { 1, 0 });
	}

	bool DrawBool(const char* label, bool& value)
	{
		bool changed = false;

		ImGui::PushID(label);

		ImGui::Columns(2, nullptr, false);
		ImGui::SetColumnWidth(0, 90.0f);

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(label);
		ImGui::NextColumn();

		changed |= ImGui::Checkbox("##value", &value);

		ImGui::Columns(1);
		ImGui::PopID();

		return changed;
	}

	bool DrawInt(const char* label, int& value, float speed, int min, int max)
	{
		bool changed = false;

		ImGui::PushID(label);

		ImGui::Columns(2, nullptr, false);
		ImGui::SetColumnWidth(0, 90.0f);

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(label);
		ImGui::NextColumn();

		changed |= ImGui::DragInt("##value", &value, speed, min, max);

		ImGui::Columns(1);
		ImGui::PopID();

		return changed;
	}

	bool DrawFloat(const char* label, float& value, float speed, float min, float max, const char* format)
	{
		bool changed = false;

		ImGui::PushID(label);

		ImGui::Columns(2, nullptr, false);
		ImGui::SetColumnWidth(0, 90.0f);

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(label);
		ImGui::NextColumn();

		changed |= ImGui::DragFloat("##value", &value, speed, min, max, format);

		ImGui::Columns(1);
		ImGui::PopID();

		return changed;
	}

	bool DrawVec2(const char* label, glm::vec2& v, float speed)
	{
		bool changed = false;

		ImGui::PushID(label);

		ImGui::Columns(2, nullptr, false);
		ImGui::SetColumnWidth(0, 90.0f);

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(label);
		ImGui::NextColumn();

		float width = ImGui::CalcItemWidth();
		float spacing = ImGui::GetStyle().ItemSpacing.x;
		float itemWidth = (width - spacing) / 2.0f;

		for (int i = 0; i < 2; ++i)
		{
			ImGui::PushID(i);
			ImGui::SetNextItemWidth(itemWidth);
			changed |= ImGui::DragFloat("##v", &v[i], speed);
			ImGui::PopID();

			if (i < 1)
			{
				ImGui::SameLine();
			}
		}

		ImGui::Columns(1);
		ImGui::PopID();

		return changed;
	}

	bool DrawVec3(const char* label, glm::vec3& v, float speed)
	{
		bool changed = false;

		ImGui::PushID(label);

		ImGui::Columns(2, nullptr, false);
		ImGui::SetColumnWidth(0, 90.0f);

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(label);
		ImGui::NextColumn();

		float width = ImGui::CalcItemWidth();
		float spacing = ImGui::GetStyle().ItemSpacing.x;
		float itemWidth = (width - spacing * 2) / 3.0f;

		for (int i = 0; i < 3; ++i)
		{
			ImGui::PushID(i);
			ImGui::SetNextItemWidth(itemWidth);
			changed |= ImGui::DragFloat("##v", &v[i], speed);
			ImGui::PopID();

			if (i < 2)
			{
				ImGui::SameLine();
			}
		}

		ImGui::Columns(1);
		ImGui::PopID();

		return changed;
	}

	bool DrawColorVec3(const char* label, glm::vec3& color, float speed)
	{
		bool changed = false;

		ImGui::PushID(label);

		ImGui::Columns(2, nullptr, false);
		ImGui::SetColumnWidth(0, 90.0f);

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(label);
		ImGui::NextColumn();

		float width = ImGui::CalcItemWidth();
		float spacing = ImGui::GetStyle().ItemSpacing.x;

		const float buttonSize = ImGui::GetFrameHeight();
		float fieldsWidth = width - buttonSize - spacing;
		float itemWidth = (fieldsWidth - spacing * 2.0f) / 3.0f;

		for (int i = 0; i < 3; ++i)
		{
			ImGui::PushID(i);
			ImGui::SetNextItemWidth(itemWidth);
			changed |= ImGui::DragFloat("##v", &color[i], speed, 0.0f, 1.0f, "%.3f");
			ImGui::PopID();

			if (i < 2)
			{
				ImGui::SameLine();
			}
		}

		ImGui::SameLine();

		ImVec4 col = { color.r, color.g, color.b, 1.0f };
		if (ImGui::ColorButton("##preview", col, ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoDragDrop, ImVec2(buttonSize, buttonSize)))
		{
			ImGui::OpenPopup("ColorPickerPopup");
		}

		if (ImGui::BeginPopup("ColorPickerPopup"))
		{
			changed |= ImGui::ColorPicker3("##picker", &color.x, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_Float);
			ImGui::EndPopup();
		}

		ImGui::Columns(1);
		ImGui::PopID();

		return changed;
	}

	bool DrawColorVec4(const char* label, glm::vec4& color, float speed)
	{
		bool changed = false;

		ImGui::PushID(label);

		ImGui::Columns(2, nullptr, false);
		ImGui::SetColumnWidth(0, 90.0f);

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(label);
		ImGui::NextColumn();

		float width = ImGui::CalcItemWidth();
		float spacing = ImGui::GetStyle().ItemSpacing.x;

		const float buttonSize = ImGui::GetFrameHeight();
		float fieldsWidth = width - buttonSize - spacing;
		float itemWidth = (fieldsWidth - spacing * 3.0f) / 4.0f;

		for (int i = 0; i < 4; ++i)
		{
			ImGui::PushID(i);
			ImGui::SetNextItemWidth(itemWidth);
			changed |= ImGui::DragFloat("##v", &color[i], speed, 0.0f, 1.0f, "%.3f");
			ImGui::PopID();

			if (i < 3)
			{
				ImGui::SameLine();
			}
		}

		ImGui::SameLine();

		if (ImGui::ColorButton("##preview", ImVec4(color.r, color.g, color.b, color.a), ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoTooltip, ImVec2(buttonSize, buttonSize)))
		{
			ImGui::OpenPopup("ColorPickerPopup");
		}

		if (ImGui::BeginPopup("ColorPickerPopup"))
		{
			changed |= ImGui::ColorPicker4("##picker", &color.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview);
			ImGui::EndPopup();
		}

		ImGui::Columns(1);
		ImGui::PopID();

		return changed;
	}

	bool DrawLabel(const char* label, const char* value, bool isDisabled)
	{
		ImGui::PushID(label);

		ImGui::Columns(2, nullptr, false);
		ImGui::SetColumnWidth(0, 90.0f);

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(label);
		ImGui::NextColumn();

		const char* text = value ? value : "<none>";

		ImGui::AlignTextToFramePadding();
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetStyle().FramePadding.x);

		if (isDisabled)
		{
			ImGui::TextDisabled("%s", text);
		}
		else
		{
			ImGui::Text("%s", text);
		}

		if (ImGui::IsItemHovered() && ImGui::CalcTextSize(text).x > ImGui::GetContentRegionAvail().x)
		{
			ImGui::SetTooltip("%s", text);
		}

		ImGui::Columns(1);
		ImGui::PopID();

		return false;
	}

	bool DrawTextInput(const char* label, std::string& value, size_t bufferSize)
	{
		bool changed = false;

		ImGui::PushID(label);

		ImGui::Columns(2, nullptr, false);
		ImGui::SetColumnWidth(0, 90.0f);

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(label);
		ImGui::NextColumn();

		ImGui::SetNextItemWidth(-1);

		changed |= ImGui::InputText("##value", &value);

		ImGui::Columns(1);
		ImGui::PopID();

		return changed;
	}

	bool DrawTextArea(const char* label, std::string& value, float height)
	{
		bool changed = false;

		ImGui::PushID(label);

		ImGui::Columns(2, nullptr, false);
		ImGui::SetColumnWidth(0, 90.0f);

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(label);
		ImGui::NextColumn();

		changed |= ImGui::InputTextMultiline("##value", &value, ImVec2(-1.0f, height));

		ImGui::Columns(1);
		ImGui::PopID();

		return changed;
	}

	bool DrawCombo(const char* label, int& value, const char* const* names, int count)
	{
		bool changed = false;

		ImGui::PushID(label);

		ImGui::Columns(2, nullptr, false);
		ImGui::SetColumnWidth(0, 90.0f);

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(label);
		ImGui::NextColumn();

		ImGui::SetNextItemWidth(-1);

		const char* preview = value >= 0 && value < count ? names[value] : "";

		if (ImGui::BeginCombo("##value", preview))
		{
			for (int index = 0; index < count; index++)
			{
				bool selected = index == value;
				if (ImGui::Selectable(names[index], selected))
				{
					value = index;
					changed = true;
				}

				if (selected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}

		ImGui::Columns(1);
		ImGui::PopID();

		return changed;
	}

	bool DrawAssetPicker(const char* label, uint64_t assetId, seri::asset::AssetType assetType, uint64_t& selection)
	{
		bool changed = false;

		ImGui::PushID(label);

		ImGui::Columns(2, nullptr, false);
		ImGui::SetColumnWidth(0, 90.0f);

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(label);
		ImGui::NextColumn();

		std::string assetName = assetId != 0 ? seri::asset::AssetManager::GetAssetName(assetId) : "<none>";
		if (ImGui::Button(assetName.c_str(), ImVec2(-1, 0)))
		{
			ImGui::OpenPopup("AssetPickerPopup");
		}

		bool selected = false;
		changed |= ShowEditorAssetPickerPopup(assetType, selected, selection);

		ImGui::Columns(1);
		ImGui::PopID();

		return changed;
	}
}
