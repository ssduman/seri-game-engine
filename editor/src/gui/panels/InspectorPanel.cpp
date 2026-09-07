#include "gui/panels/InspectorPanel.h"
#include "gui/EditorWidgets.h"

namespace seri::editor
{
	void InspectorPanel::Draw(GUIContext& ctx)
	{
		if (ctx.selectedEntityId == 0)
		{
			ImGui::Text("no entity selected");
			return;
		}

		if (ctx.inspectorType == InspectorType::none)
		{
			ImGui::Text("no inspector type selected");
			return;
		}

		switch (ctx.inspectorType)
		{
			case InspectorType::scene:
				{
					DrawScene();
				}
				break;
			case InspectorType::entity:
				{
					DrawEntity(ctx);
				}
				break;
			case InspectorType::asset:
				{
					DrawAsset(ctx);
				}
				break;
			default:
				{
					LIB_LOGGER(error, gui) << "editor unknown inspector type";
				}
				break;
		}
	}

	void InspectorPanel::DrawScene()
	{
		auto scene = seri::scene::SceneManager::GetActiveScene();

		auto idComponent = scene->GetIDComponent();
		{
			ImGui::Text("id: %llu", idComponent.id);
			ImGui::Text("parent id: %llu", idComponent.parentId);
			ImGui::Text("name: %s", idComponent.name.c_str());

			ImGui::Separator();
		}

		auto sceneComponent = scene->GetSceneComponent();
		{
			ImGui::Text("version: %s", sceneComponent.version.c_str());
			ImGui::Text("active: %s", sceneComponent.isActive ? "true" : "false");

			ImGui::Separator();
		}
	}

	void InspectorPanel::DrawEntity(GUIContext& ctx)
	{
		auto scene = seri::scene::SceneManager::GetActiveScene();
		auto& registry = seri::scene::SceneManager::GetRegistry();

		auto entity = scene->GetEntityByID(ctx.selectedEntityId);

		ImGuiChildFlags childFlags = ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY;

		if (auto* idComp = registry.try_get<seri::component::IDComponent>(entity))
		{
			ScopedChild scopedChild("##IDComponent", ImVec2(0, 0), childFlags);

			ImGui::TextUnformatted("Entity");
			ImGui::Separator();

			bool changed = false;

			changed |= DrawTextInput("Name", idComp->name);
			changed |= DrawLabel("ID", std::to_string(idComp->id).c_str(), true);
			changed |= DrawLabel("Parent ID", idComp->parentId != 0 ? std::to_string(idComp->parentId).c_str() : "<none>", true);

			if (changed)
			{
				scene->SetAsDirty();
			}
		}

		if (auto* transformComp = registry.try_get<seri::component::TransformComponent>(entity))
		{
			ScopedChild scopedChild("##TransformComponent", ImVec2(0, 0), childFlags);

			ImGui::TextUnformatted("Transform Component");
			ImGui::Separator();

			bool changed = false;

			changed |= DrawVec3("Position", transformComp->position, 0.1f);
			changed |= DrawVec3("Rotation", transformComp->rotation, 0.5f);
			changed |= DrawVec3("Scale", transformComp->scale, 0.1f);

			if (changed)
			{
				scene->SetAsDirty();
			}
		}

		if (auto* meshComp = registry.try_get<seri::component::MeshComponent>(entity))
		{
			ScopedChild scopedChild("##MeshComponent", ImVec2(0, 0), childFlags);

			ImGui::TextUnformatted("Mesh Component");
			ImGui::Separator();

			bool changed = false;
			uint64_t selection = 0;

			changed |= DrawAssetPicker("Mesh", meshComp->meshAssetId, seri::asset::AssetType::mesh, selection);

			if (changed)
			{
				scene->SetAsDirty();
				meshComp->meshAssetId = selection;
			}
		}

		if (auto* meshRendererComp = registry.try_get<seri::component::MeshRendererComponent>(entity))
		{
			ScopedChild scopedChild("##MeshRendererComponent", ImVec2(0, 0), childFlags);

			ImGui::TextUnformatted("Mesh Renderer Component");
			ImGui::Separator();

			bool changed = false;
			uint64_t selection = 0;

			changed |= DrawAssetPicker("Material", meshRendererComp->materialAssetId, seri::asset::AssetType::material, selection);

			if (changed)
			{
				scene->SetAsDirty();
				meshRendererComp->materialAssetId = selection;
			}
		}

		if (auto* skinnedMeshRendererComp = registry.try_get<seri::component::SkinnedMeshRendererComponent>(entity))
		{
			ScopedChild scopedChild("##SkinnedMeshRendererComponent", ImVec2(0, 0), childFlags);

			ImGui::TextUnformatted("Skinned Mesh Renderer Component");
			ImGui::Separator();

			bool changed = false;
			uint64_t selection = 0;

			if (DrawAssetPicker("Mesh", skinnedMeshRendererComp->meshAssetId, seri::asset::AssetType::mesh, selection))
			{
				skinnedMeshRendererComp->meshAssetId = selection;
				changed = true;
			}

			ImGui::TextUnformatted("Materials");

			for (size_t i = 0; i < skinnedMeshRendererComp->materialAssetIds.size(); i++)
			{
				ImGui::PushID(static_cast<int>(i));

				std::string label = fmt::format("Element {}", i);

				if (DrawAssetPicker(label.c_str(), skinnedMeshRendererComp->materialAssetIds[i], seri::asset::AssetType::material, selection))
				{
					skinnedMeshRendererComp->materialAssetIds[i] = selection;
					changed = true;
				}

				ImGui::PopID();
			}

			if (ImGui::Button("Add Material"))
			{
				skinnedMeshRendererComp->materialAssetIds.push_back(0);
				changed = true;
			}

			ImGui::SameLine();

			if (ImGui::Button("Remove Material") && !skinnedMeshRendererComp->materialAssetIds.empty())
			{
				skinnedMeshRendererComp->materialAssetIds.pop_back();
				changed = true;
			}

			if (changed)
			{
				scene->SetAsDirty();
			}
		}

		if (auto* animatorComp = registry.try_get<seri::component::AnimatorComponent>(entity))
		{
			ScopedChild scopedChild("##AnimatorComponent", ImVec2(0, 0), childFlags);

			ImGui::TextUnformatted("Animator Component");
			ImGui::Separator();

			bool changed = false;

			changed |= DrawBool("Playing", animatorComp->playing);
			changed |= DrawBool("Loop", animatorComp->loop);
			changed |= DrawFloat("Speed", animatorComp->speed, 0.05f, -10.0f, 10.0f);

			DrawLabel("Time", fmt::format("{:.3f}", animatorComp->time).c_str(), true);

			if (changed)
			{
				scene->SetAsDirty();
			}
		}

		if (auto* cameraComp = registry.try_get<seri::component::CameraComponent>(entity))
		{
			ScopedChild scopedChild("##CameraComponent", ImVec2(0, 0), childFlags);

			ImGui::TextUnformatted("Camera Component");
			ImGui::Separator();

			bool changed = false;

			changed |= DrawBool("Main", cameraComp->isMain);
			changed |= DrawBool("Orthographic", cameraComp->isOrtho);

			if (cameraComp->isOrtho)
			{
				changed |= DrawFloat("Size", cameraComp->orthoSize, 0.1f, 0.01f, 1000.0f);
			}
			else
			{
				changed |= DrawFloat("Field Of View", cameraComp->fov, 0.1f, 1.0f, 179.0f);
			}

			changed |= DrawFloat("Near Plane", cameraComp->nearPlane, 0.01f, 0.001f, 1000.0f);
			changed |= DrawFloat("Far Plane", cameraComp->farPlane, 1.0f, 0.01f, 10000.0f);

			if (changed)
			{
				scene->SetAsDirty();
			}

			if (seri::scene::SceneManager::GetState() == seri::scene::SceneState::edit)
			{
				seri::system::CameraSystem::DrawFrustum(entity);
			}
		}

		if (auto* dirLightComp = registry.try_get<seri::component::DirectionalLightComponent>(entity))
		{
			ScopedChild scopedChild("##DirectionalLightComponent", ImVec2(0, 0), childFlags);

			ImGui::TextUnformatted("Directional Light Component");
			ImGui::Separator();

			bool changed = false;

			changed |= DrawFloat("Intensity", dirLightComp->intensity, 0.1f, 0.0f, 200.0f);
			changed |= DrawColorVec3("Color", dirLightComp->color, 0.05f);

			if (changed)
			{
				scene->SetAsDirty();
			}
		}

		if (auto* spotLightComp = registry.try_get<seri::component::SpotLightComponent>(entity))
		{
			ScopedChild scopedChild("##SpotLightComponent", ImVec2(0, 0), childFlags);

			ImGui::TextUnformatted("Spot Light Component");
			ImGui::Separator();

			bool changed = false;

			changed |= DrawFloat("Intensity", spotLightComp->intensity, 0.1f, 0.0f, 200.0f);
			changed |= DrawFloat("Inner Angle", spotLightComp->innerAngle, 0.1f, 0.0f, 360.0f);
			changed |= DrawFloat("Outer Angle", spotLightComp->outerAngle, 0.1f, 0.0f, 360.0f);
			changed |= DrawBool("Cast Shadow", spotLightComp->castShadow);
			changed |= DrawColorVec3("Color", spotLightComp->color, 0.05f);

			if (changed)
			{
				scene->SetAsDirty();
			}
		}

		if (auto* pointLightComp = registry.try_get<seri::component::PointLightComponent>(entity))
		{
			ScopedChild scopedChild("##PointLightComponent", ImVec2(0, 0), childFlags);

			ImGui::TextUnformatted("Point Light Component");
			ImGui::Separator();

			bool changed = false;

			changed |= DrawFloat("Intensity", pointLightComp->intensity, 0.1f, 0.0f, 200.0f);
			changed |= DrawFloat("Range", pointLightComp->range, 0.1f, 0.0f, 1000.0f);
			changed |= DrawColorVec3("Color", pointLightComp->color, 0.05f);

			if (changed)
			{
				scene->SetAsDirty();
			}
		}

		if (auto* scriptComp = registry.try_get<seri::component::ScriptComponent>(entity))
		{
			ScopedChild scopedChild("##ScriptComponent", ImVec2(0, 0), childFlags);

			ImGui::TextUnformatted("Script Component");
			ImGui::Separator();

			bool changed = false;
			bool rebuild = false;
			int removeIndex = -1;

			for (size_t i = 0; i < scriptComp->entries.size(); i++)
			{
				auto& entry = scriptComp->entries[i];

				ImGui::PushID(static_cast<int>(i));

				changed |= ImGui::Checkbox("##enabled", &entry.enabled);
				ImGui::SameLine();

				const bool known = seri::script::ScriptRegistry::Contains(entry.name);
				if (!known)
				{
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.4f, 0.4f, 1.0f));
				}
				ImGui::AlignTextToFramePadding();
				ImGui::TextUnformatted(entry.name.empty() ? "<none>" : entry.name.c_str());
				if (!known)
				{
					ImGui::PopStyleColor();
					if (ImGui::IsItemHovered())
					{
						ImGui::SetTooltip("script is not registered");
					}
				}

				ImGui::SameLine(ImGui::GetContentRegionAvail().x - 10.0f);
				if (ImGui::SmallButton("X"))
				{
					removeIndex = static_cast<int>(i);
				}

				ImGui::Indent();

				for (const auto& field : seri::script::ScriptSystem::GetSerializedFields(entity, i))
				{
					bool fieldChanged = false;

					ImGui::PushID(field.name.c_str());
					ImGui::SetNextItemWidth(-1);

					switch (field.type)
					{
						case seri::script::ScriptField::Type::boolean:
							fieldChanged = DrawBool(field.name.c_str(), *static_cast<bool*>(field.ptr));
							break;
						case seri::script::ScriptField::Type::integer:
							fieldChanged = DrawInt(field.name.c_str(), *static_cast<int*>(field.ptr));
							break;
						case seri::script::ScriptField::Type::floating:
							fieldChanged = DrawFloat(field.name.c_str(), *static_cast<float*>(field.ptr));
							break;
						case seri::script::ScriptField::Type::vec3:
							fieldChanged = DrawVec3(field.name.c_str(), *static_cast<glm::vec3*>(field.ptr), 0.1f);
							break;
						case seri::script::ScriptField::Type::color3:
							fieldChanged = DrawColorVec3(field.name.c_str(), *static_cast<glm::vec3*>(field.ptr), 0.05f);
							break;
						case seri::script::ScriptField::Type::text:
							fieldChanged = DrawTextInput(field.name.c_str(), *static_cast<std::string*>(field.ptr));
							break;
					}

					ImGui::PopID();

					if (fieldChanged)
					{
						seri::script::ScriptSystem::OverrideFields(entity, i);
						changed = true;
					}
				}

				ImGui::Unindent();

				ImGui::PopID();
			}

			if (removeIndex >= 0)
			{
				scriptComp->entries.erase(scriptComp->entries.begin() + removeIndex);
				rebuild = true;
			}

			ImGui::Spacing();

			if (ImGui::Button("Add Script", ImVec2(-1, 0)))
			{
				ImGui::OpenPopup("ScriptPickerPopup");
			}

			std::string selection;
			if (ShowEditorScriptPickerPopup(selection))
			{
				scriptComp->entries.push_back({ selection, true });
				rebuild = true;
			}

			if (rebuild)
			{
				scriptComp->dirty = true;
			}

			if (changed || rebuild)
			{
				scene->SetAsDirty();
			}
		}

		ImGui::NewLine();

		if (ImGui::Button("Add Component", ImVec2(-1, 0)))
		{
			ImGui::OpenPopup("AddComponentPopup");
		}
		ShowComponentPickerPopup(ctx);
	}

	void InspectorPanel::DrawAsset(GUIContext& ctx)
	{
		switch (ctx.selectedAsset.type)
		{
			case seri::asset::AssetType::material:
				{
					DrawAssetMaterial(ctx);
				}
				break;
			case seri::asset::AssetType::shader:
				{
					auto asset = seri::asset::AssetManager::GetAssetByID<seri::ShaderBase>(ctx.selectedAsset.id);
					if (!asset)
					{
						ImGui::TextDisabled("shader not loaded");
						break;
					}
					ImGui::Text("shader: %llu", asset->id);
				}
				break;
			case seri::asset::AssetType::texture:
				{
					auto asset = seri::asset::AssetManager::GetAssetByID<seri::TextureBase>(ctx.selectedAsset.id);
					if (!asset)
					{
						ImGui::TextDisabled("texture not loaded");
						break;
					}
					ImGui::Text("texture: %llu", asset->id);
					ShowEditorImage(asset, 128.0f);
				}
				break;
			case seri::asset::AssetType::mesh:
				{
					auto asset = seri::asset::AssetManager::GetAssetByID<seri::Model>(ctx.selectedAsset.id);
					if (!asset)
					{
						ImGui::TextDisabled("mesh not loaded");
						break;
					}
					ImGui::Text("mesh: %llu", asset->id);
					ImGui::Text("material count: %d", asset->materialCount);
					for (auto& mesh : asset->meshes)
					{
						ImGui::Text(" mesh: %s, mat: %d", mesh->name.c_str(), mesh->materialIndex);
					}
				}
				break;
			default:
				{
					ImGui::Text("other asset");
				}
				break;
		}
	}

	void InspectorPanel::DrawAssetMaterial(GUIContext& ctx)
	{
		constexpr float previewSize = 64.0f;
		constexpr float labelWidth = 150.0f;

		auto asset = seri::asset::AssetManager::GetAssetByID<seri::Material>(ctx.selectedAsset.id);
		if (!asset)
		{
			return;
		}

		ImGui::PushID("material_inspector");

		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 8));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6, 4));

		auto Section = [](const char* title)
			{
				ImGui::Spacing();
				ImGui::Separator();
				ImGui::TextUnformatted(title);
				ImGui::Spacing();
			};

		auto PropertyRow = [&](std::string_view label)
			{
				ImGui::PushID(label.data());
				ImGui::Columns(2, nullptr, false);
				ImGui::SetColumnWidth(0, labelWidth);
				ImGui::AlignTextToFramePadding();
				ImGui::TextUnformatted(label.data());
				ImGui::NextColumn();
				ImGui::SetNextItemWidth(-1);
			};

		auto EndPropertyRow = []()
			{
				ImGui::Columns(1);
				ImGui::PopID();
			};

		ImGui::TextUnformatted("Material");
		ImGui::Separator();

		ImGui::TextDisabled("ID: %llu", asset->id);
		ImGui::TextDisabled("Shader: %llu", asset->GetShader() ? asset->GetShader()->id : 0);

		{
			ImGui::Separator();

			PropertyRow("Shader");

			auto shader = asset->GetShader();
			std::string shaderName = (!shader || shader->id == 0) ? "<None>" : seri::asset::AssetManager::GetAssetName(shader->id);
			if (ImGui::Button(shaderName.c_str()))
			{
				ImGui::OpenPopup("AssetPickerPopup");
			}

			bool selected = false;
			uint64_t selection = 0;
			if (ShowEditorAssetPickerPopup(seri::asset::AssetType::shader, selected, selection))
			{
				if (selected)
				{
					auto newShader = seri::asset::AssetManager::GetAssetByID<seri::ShaderBase>(selection);
					asset->SetShader(newShader);
				}
			}

			EndPropertyRow();
		}

		if (!asset->GetTextures().empty())
		{
			ImGui::Separator();

			for (auto& tex : asset->GetTextures())
			{
				ImGui::PushID(tex.first.c_str());

				ImGui::Columns(2, nullptr, false);
				ImGui::SetColumnWidth(0, labelWidth);
				ImGui::AlignTextToFramePadding();
				ImGui::TextUnformatted(tex.first.c_str());
				ImGui::NextColumn();

				bool clicked = false;
				if (tex.second)
				{
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
					clicked = ShowEditorImageButton(tex.second, previewSize - 8.0f);
					ImGui::PopStyleVar();
				}
				else
				{
					ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_FrameBg));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyleColorVec4(ImGuiCol_FrameBgHovered));
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::GetStyleColorVec4(ImGuiCol_FrameBgActive));

					clicked = ImGui::Button("None", ImVec2(previewSize, previewSize));

					ImGui::PopStyleColor(3);
				}
				if (clicked)
				{
					ImGui::OpenPopup("AssetPickerPopup");
				}

				bool selected = false;
				uint64_t selection = 0;
				if (ShowEditorAssetPickerPopup(seri::asset::AssetType::texture, selected, selection))
				{
					if (selected)
					{
						auto newTex = seri::asset::AssetManager::GetAssetByID<seri::TextureBase>(selection);
						asset->SetTexture(tex.first, newTex);
					}
				}

				if (tex.second)
				{
					ImGui::SameLine();
					if (ImGui::SmallButton("X"))
					{
						asset->SetTexture(tex.first, nullptr);
					}
				}

				ImGui::Columns(1);
				ImGui::Spacing();
				ImGui::PopID();
			}
		}

		if (!asset->GetBools().empty())
		{
			for (auto& kv : asset->GetBools())
			{
				PropertyRow(kv.first);
				ImGui::Checkbox("##bool", &kv.second);
				EndPropertyRow();
			}
		}

		if (!asset->GetInts().empty() || !asset->GetInt2s().empty() || !asset->GetInt3s().empty() || !asset->GetInt4s().empty())
		{
			for (auto& kv : asset->GetInts())
			{
				PropertyRow(kv.first);
				ImGui::DragInt("##int", &kv.second);
				EndPropertyRow();
			}
			for (auto& kv : asset->GetInt2s())
			{
				PropertyRow(kv.first);
				ImGui::DragInt2("##int2", glm::value_ptr(kv.second));
				EndPropertyRow();
			}
			for (auto& kv : asset->GetInt3s())
			{
				PropertyRow(kv.first);
				ImGui::DragInt3("##int3", glm::value_ptr(kv.second));
				EndPropertyRow();
			}
			for (auto& kv : asset->GetInt4s())
			{
				PropertyRow(kv.first);
				ImGui::DragInt4("##int4", glm::value_ptr(kv.second));
				EndPropertyRow();
			}
		}

		if (!asset->GetFloats().empty() || !asset->GetFloat2s().empty() || !asset->GetFloat3s().empty() || !asset->GetFloat4s().empty())
		{
			for (auto& kv : asset->GetFloats())
			{
				PropertyRow(kv.first);
				ImGui::DragFloat("##float", &kv.second, 0.01f);
				EndPropertyRow();
			}
			for (auto& kv : asset->GetFloat2s())
			{
				PropertyRow(kv.first);
				ImGui::DragFloat2("##float2", glm::value_ptr(kv.second), 0.01f);
				EndPropertyRow();
			}
			for (auto& kv : asset->GetFloat3s())
			{
				if (Util::IsIgnoredUniform(kv.first))
				{
					continue;
				}
				PropertyRow(kv.first);
				ImGui::DragFloat3("##float3", glm::value_ptr(kv.second), 0.01f);
				EndPropertyRow();
			}
			for (auto& kv : asset->GetFloat4s())
			{
				if (Util::IsIgnoredUniform(kv.first))
				{
					continue;
				}
				PropertyRow(kv.first);
				ImGui::DragFloat4("##float4", glm::value_ptr(kv.second), 0.01f);
				EndPropertyRow();
			}
		}

		ImGui::PopStyleVar(3);
		ImGui::PopID();
	}

	void InspectorPanel::ShowComponentPickerPopup(GUIContext& ctx)
	{
		static char search[128] = "";

		auto scene = seri::scene::SceneManager::GetActiveScene();
		auto entity = scene->GetEntityByID(ctx.selectedEntityId);

		if (ImGui::BeginPopup("AddComponentPopup"))
		{
			ImGui::TextUnformatted("Add Component");
			ImGui::Separator();

			ImGui::InputTextWithHint("##Search", "Search components...", search, IM_ARRAYSIZE(search));

			ImGui::Spacing();

			for (const auto& info : seri::scene::SceneManager::GetCompIO())
			{
				if (!Util::ContainsIgnoreCase(info.name, search))
				{
					continue;
				}

				std::string compName = std::string(info.name);

				ImGui::PushID(compName.c_str());

				if (ImGui::Selectable(compName.c_str()))
				{
					seri::scene::SceneManager::AddComponent(entity, info.name);
					ImGui::CloseCurrentPopup();
					search[0] = '\0';
				}

				ImGui::PopID();
			}

			ImGui::EndPopup();
		}
	}
}
