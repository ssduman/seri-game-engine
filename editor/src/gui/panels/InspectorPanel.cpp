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

		std::string_view removeComp{};

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

			changed |= DrawBool("Active", transformComp->isActive);
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

			if (DrawComponentHeader("Mesh Component"))
			{
				removeComp = seri::component::MeshComponent::kCompName;
			}

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

			if (DrawComponentHeader("Mesh Renderer Component"))
			{
				removeComp = seri::component::MeshRendererComponent::kCompName;
			}

			bool changed = false;
			uint64_t selection = 0;

			changed |= DrawBool("Cast Shadow", meshRendererComp->castShadow);

			std::shared_ptr<seri::Model> model;
			if (auto* meshComp = registry.try_get<seri::component::MeshComponent>(entity))
			{
				model = seri::asset::AssetManager::GetAssetByID<seri::Model>(meshComp->meshAssetId);
			}

			ImGui::TextUnformatted("Materials");

			for (size_t i = 0; i < meshRendererComp->materialAssetIds.size(); i++)
			{
				ImGui::PushID(static_cast<int>(i));

				std::string label = GetMaterialSlotLabel(model, i);

				if (DrawAssetPicker(label.c_str(), meshRendererComp->materialAssetIds[i], seri::asset::AssetType::material, selection))
				{
					meshRendererComp->materialAssetIds[i] = selection;
					changed = true;
				}

				ImGui::PopID();
			}

			if (ImGui::Button("Add Material"))
			{
				meshRendererComp->materialAssetIds.push_back(0);
				changed = true;
			}

			ImGui::SameLine();

			if (ImGui::Button("Remove Material") && !meshRendererComp->materialAssetIds.empty())
			{
				meshRendererComp->materialAssetIds.pop_back();
				changed = true;
			}

			if (model && model->materialCount != static_cast<int>(meshRendererComp->materialAssetIds.size()))
			{
				ImGui::SameLine();
				if (ImGui::Button("Fit"))
				{
					meshRendererComp->materialAssetIds.resize(model->materialCount, 0);
					changed = true;
				}
			}

			if (changed)
			{
				scene->SetAsDirty();
			}
		}

		if (auto* skinnedMeshRendererComp = registry.try_get<seri::component::SkinnedMeshRendererComponent>(entity))
		{
			ScopedChild scopedChild("##SkinnedMeshRendererComponent", ImVec2(0, 0), childFlags);

			if (DrawComponentHeader("Skinned Mesh Renderer Component"))
			{
				removeComp = seri::component::SkinnedMeshRendererComponent::kCompName;
			}

			bool changed = false;
			uint64_t selection = 0;

			if (DrawAssetPicker("Mesh", skinnedMeshRendererComp->meshAssetId, seri::asset::AssetType::mesh, selection))
			{
				skinnedMeshRendererComp->meshAssetId = selection;
				changed = true;
			}

			auto skinnedModel = seri::asset::AssetManager::GetAssetByID<seri::Model>(skinnedMeshRendererComp->meshAssetId);

			changed |= DrawBool("Cast Shadow", skinnedMeshRendererComp->castShadow);

			ImGui::TextUnformatted("Materials");

			for (size_t i = 0; i < skinnedMeshRendererComp->materialAssetIds.size(); i++)
			{
				ImGui::PushID(static_cast<int>(i));

				std::string label = GetMaterialSlotLabel(skinnedModel, i);

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

		if (auto* spriteRendererComp = registry.try_get<seri::component::SpriteRendererComponent>(entity))
		{
			ScopedChild scopedChild("##SpriteRendererComponent", ImVec2(0, 0), childFlags);

			if (DrawComponentHeader("Sprite Renderer Component"))
			{
				removeComp = seri::component::SpriteRendererComponent::kCompName;
			}

			bool changed = false;
			uint64_t selection = 0;

			if (DrawAssetPicker("Texture", spriteRendererComp->textureAssetId, seri::asset::AssetType::texture, selection))
			{
				spriteRendererComp->textureAssetId = selection;
				changed = true;
			}

			changed |= DrawColorVec4("Color", spriteRendererComp->color, 0.01f);
			changed |= DrawBool("Flip X", spriteRendererComp->flipX);
			changed |= DrawBool("Flip Y", spriteRendererComp->flipY);

			if (changed)
			{
				scene->SetAsDirty();
			}
		}

		if (auto* audioComp = registry.try_get<seri::component::AudioComponent>(entity))
		{
			ScopedChild scopedChild("##AudioComponent", ImVec2(0, 0), childFlags);

			if (DrawComponentHeader("Audio Component"))
			{
				removeComp = seri::component::AudioComponent::kCompName;
			}

			bool changed = false;

			changed |= DrawTextInput("Sound", audioComp->soundPath);
			changed |= DrawFloat("Volume", audioComp->volume, 0.01f, 0.0f, 1.0f);
			changed |= DrawBool("Loop", audioComp->loop);
			changed |= DrawBool("Play On Start", audioComp->playOnStart);

			if (changed)
			{
				scene->SetAsDirty();
			}
		}

		if (auto* animatorComp = registry.try_get<seri::component::AnimatorComponent>(entity))
		{
			ScopedChild scopedChild("##AnimatorComponent", ImVec2(0, 0), childFlags);

			if (DrawComponentHeader("Animator Component"))
			{
				removeComp = seri::component::AnimatorComponent::kCompName;
			}

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

			if (DrawComponentHeader("Camera Component"))
			{
				removeComp = seri::component::CameraComponent::kCompName;
			}

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
				seri::system::CameraSystem::DrawFrustumGizmo(entity);
			}
		}

		if (auto* dirLightComp = registry.try_get<seri::component::DirectionalLightComponent>(entity))
		{
			ScopedChild scopedChild("##DirectionalLightComponent", ImVec2(0, 0), childFlags);

			if (DrawComponentHeader("Directional Light Component"))
			{
				removeComp = seri::component::DirectionalLightComponent::kCompName;
			}

			bool changed = false;

			changed |= DrawFloat("Intensity", dirLightComp->intensity, 0.1f, 0.0f, 200.0f);
			changed |= DrawColorVec3("Color", dirLightComp->color, 0.05f);

			if (changed)
			{
				scene->SetAsDirty();
			}

			if (seri::scene::SceneManager::GetState() == seri::scene::SceneState::edit)
			{
				seri::system::LightSystem::DrawDirectionalLightGizmo(entity);
			}
		}

		if (auto* spotLightComp = registry.try_get<seri::component::SpotLightComponent>(entity))
		{
			ScopedChild scopedChild("##SpotLightComponent", ImVec2(0, 0), childFlags);

			if (DrawComponentHeader("Spot Light Component"))
			{
				removeComp = seri::component::SpotLightComponent::kCompName;
			}

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

			if (seri::scene::SceneManager::GetState() == seri::scene::SceneState::edit)
			{
				seri::system::LightSystem::DrawSpotLightGizmo(entity);
			}
		}

		if (auto* pointLightComp = registry.try_get<seri::component::PointLightComponent>(entity))
		{
			ScopedChild scopedChild("##PointLightComponent", ImVec2(0, 0), childFlags);

			if (DrawComponentHeader("Point Light Component"))
			{
				removeComp = seri::component::PointLightComponent::kCompName;
			}

			bool changed = false;

			changed |= DrawFloat("Intensity", pointLightComp->intensity, 0.1f, 0.0f, 200.0f);
			changed |= DrawFloat("Range", pointLightComp->range, 0.1f, 0.0f, 1000.0f);
			changed |= DrawColorVec3("Color", pointLightComp->color, 0.05f);

			if (changed)
			{
				scene->SetAsDirty();
			}

			if (seri::scene::SceneManager::GetState() == seri::scene::SceneState::edit)
			{
				seri::system::LightSystem::DrawPointLightGizmo(entity);
			}
		}

		if (auto* textComp = registry.try_get<seri::component::TextComponent>(entity))
		{
			ScopedChild scopedChild("##TextComponent", ImVec2(0, 0), childFlags);

			if (DrawComponentHeader("Text Component"))
			{
				removeComp = seri::component::TextComponent::kCompName;
			}

			bool changed = false;
			uint64_t selection = 0;

			if (DrawAssetPicker("Font", textComp->fontAssetId, seri::asset::AssetType::font, selection))
			{
				textComp->fontAssetId = selection;
				changed = true;
			}

			changed |= DrawTextArea("Text", textComp->text, 60.0f);
			changed |= DrawFloat("Font Size", textComp->fontSize, 0.1f, 0.001f, 1000.0f);
			changed |= DrawFloat("Line Spacing", textComp->lineSpacing, 0.1f, 0.01f, 10.0f);
			changed |= DrawColorVec4("Color", textComp->color, 0.01f);

			static const char* alignHNames[] = {
				seri::font::TextAlignHToString(seri::font::TextAlignH::left),
				seri::font::TextAlignHToString(seri::font::TextAlignH::center),
				seri::font::TextAlignHToString(seri::font::TextAlignH::right),
			};
			static const char* alignVNames[] = {
				seri::font::TextAlignVToString(seri::font::TextAlignV::top),
				seri::font::TextAlignVToString(seri::font::TextAlignV::middle),
				seri::font::TextAlignVToString(seri::font::TextAlignV::bottom),
				seri::font::TextAlignVToString(seri::font::TextAlignV::baseline),
			};

			int alignH = static_cast<int>(textComp->alignH);
			if (DrawCombo("Align H", alignH, alignHNames, IM_ARRAYSIZE(alignHNames)))
			{
				textComp->alignH = static_cast<seri::font::TextAlignH>(alignH);
				changed = true;
			}

			int alignV = static_cast<int>(textComp->alignV);
			if (DrawCombo("Align V", alignV, alignVNames, IM_ARRAYSIZE(alignVNames)))
			{
				textComp->alignV = static_cast<seri::font::TextAlignV>(alignV);
				changed = true;
			}

			if (changed)
			{
				scene->SetAsDirty();
			}
		}

		if (auto* scriptComp = registry.try_get<seri::component::ScriptComponent>(entity))
		{
			ScopedChild scopedChild("##ScriptComponent", ImVec2(0, 0), childFlags);

			if (DrawComponentHeader("Script Component"))
			{
				removeComp = seri::component::ScriptComponent::kCompName;
			}

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

		if (!removeComp.empty())
		{
			seri::scene::SceneManager::RemoveComponent(entity, removeComp);
			scene->SetAsDirty();
		}
	}

	void InspectorPanel::DrawAsset(GUIContext& ctx)
	{
		DrawAssetHeader(ctx);

		if (ctx.selectedAsset.isFolder)
		{
			return;
		}

		switch (ctx.selectedAsset.type)
		{
			case seri::asset::AssetType::material:
				{
					DrawAssetMaterial(ctx);
				}
				break;
			case seri::asset::AssetType::texture:
				{
					DrawAssetTexture(ctx);
				}
				break;
			case seri::asset::AssetType::mesh:
				{
					DrawAssetMesh(ctx);
				}
				break;
			case seri::asset::AssetType::shader:
				{
					DrawAssetShader(ctx);
				}
				break;
			case seri::asset::AssetType::font:
				{
					DrawAssetFont(ctx);
				}
				break;
			default:
				break;
		}
	}

	void InspectorPanel::DrawAssetHeader(GUIContext& ctx)
	{
		const seri::asset::AssetTreeNode& node = ctx.selectedAsset;

		ScopedChild scopedChild("##AssetHeader", ImVec2(0, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY);

		ImGui::TextUnformatted(node.name.empty() ? "assets" : node.name.c_str());
		ImGui::SameLine();
		ImGui::TextDisabled("(%s)", GetAssetTypeName(node));

		ImGui::Separator();

		std::error_code ec;
		std::filesystem::path relative = std::filesystem::relative(node.path, seri::asset::AssetManager::GetWorkingDirectory(), ec);
		std::string path = ec ? node.path.string() : relative.string();

		DrawLabel("Path", path.c_str(), true);

		if (node.isFolder)
		{
			DrawLabel("Items", std::to_string(node.children.size()).c_str(), true);
			return;
		}

		DrawLabel("ID", std::to_string(node.id).c_str(), true);

		if (node.type == seri::asset::AssetType::material || node.type == seri::asset::AssetType::mesh)
		{
			if (ImGui::Button("Save Asset", ImVec2(-1, 0)))
			{
				seri::asset::AssetManager::SaveAsset(node.id);
			}
		}
	}

	void InspectorPanel::DrawAssetMaterial(GUIContext& ctx)
	{
		constexpr float previewSize = 56.0f;
		constexpr float labelWidth = 150.0f;

		auto asset = seri::asset::AssetManager::GetAssetByID<seri::Material>(ctx.selectedAsset.id);
		if (!asset)
		{
			ImGui::TextDisabled("material not loaded");
			return;
		}

		ImGui::PushID("material_inspector");

		auto PropertyRow = [&](const std::string& label)
			{
				ImGui::PushID(label.c_str());
				ImGui::Columns(2, nullptr, false);
				ImGui::SetColumnWidth(0, labelWidth);
				ImGui::AlignTextToFramePadding();
				ImGui::TextUnformatted(label.c_str());
				ImGui::NextColumn();
				ImGui::SetNextItemWidth(-1);
			};

		auto EndPropertyRow = []()
			{
				ImGui::Columns(1);
				ImGui::PopID();
			};

		{
			ScopedChild scopedChild("##MaterialShader", ImVec2(0, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY);

			ImGui::TextUnformatted("Shader");
			ImGui::Separator();

			PropertyRow("Shader");

			auto shader = asset->GetShader();
			std::string shaderName = (!shader || shader->id == 0) ? "<none>" : seri::asset::AssetManager::GetAssetName(shader->id);
			if (ImGui::Button(shaderName.c_str(), ImVec2(-1, 0)))
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
					if (newShader)
					{
						asset->SetShader(newShader->Clone());
					}
				}
			}

			EndPropertyRow();
		}

		{
			ScopedChild scopedChild("##MaterialTextures", ImVec2(0, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY);

			ImGui::TextUnformatted("Textures");
			ImGui::Separator();

			bool hasTexture = false;

			for (auto& tex : asset->GetTextures())
			{
				if (Util::IsIgnoredUniform(tex.first))
				{
					continue;
				}

				hasTexture = true;

				ImGui::PushID(tex.first.c_str());

				ImGui::Columns(2, nullptr, false);
				ImGui::SetColumnWidth(0, labelWidth);
				ImGui::AlignTextToFramePadding();
				ImGui::TextUnformatted(tex.first.c_str());

				if (tex.second)
				{
					ImGui::TextDisabled("%s", seri::asset::AssetManager::GetAssetName(tex.second->id).c_str());
				}

				ImGui::NextColumn();

				bool clicked = false;
				if (tex.second)
				{
					clicked = ShowEditorImageButton(tex.second, previewSize);
				}
				else
				{
					ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_FrameBg));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyleColorVec4(ImGuiCol_FrameBgHovered));
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::GetStyleColorVec4(ImGuiCol_FrameBgActive));

					clicked = ImGui::Button("none", ImVec2(previewSize, previewSize));

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

			if (!hasTexture)
			{
				ImGui::TextDisabled("no texture slot");
			}
		}

		{
			ScopedChild scopedChild("##MaterialProperties", ImVec2(0, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY);

			ImGui::TextUnformatted("Properties");
			ImGui::Separator();

			bool hasProperty = false;

			for (auto& kv : asset->GetBools())
			{
				if (Util::IsIgnoredUniform(kv.first))
				{
					continue;
				}
				hasProperty = true;
				PropertyRow(kv.first);
				ImGui::Checkbox("##bool", &kv.second);
				EndPropertyRow();
			}

			for (auto& kv : asset->GetInts())
			{
				if (Util::IsIgnoredUniform(kv.first))
				{
					continue;
				}
				hasProperty = true;
				PropertyRow(kv.first);
				ImGui::DragInt("##int", &kv.second);
				EndPropertyRow();
			}

			for (auto& kv : asset->GetInt2s())
			{
				if (Util::IsIgnoredUniform(kv.first))
				{
					continue;
				}
				hasProperty = true;
				PropertyRow(kv.first);
				ImGui::DragInt2("##int2", glm::value_ptr(kv.second));
				EndPropertyRow();
			}

			for (auto& kv : asset->GetInt3s())
			{
				if (Util::IsIgnoredUniform(kv.first))
				{
					continue;
				}
				hasProperty = true;
				PropertyRow(kv.first);
				ImGui::DragInt3("##int3", glm::value_ptr(kv.second));
				EndPropertyRow();
			}

			for (auto& kv : asset->GetInt4s())
			{
				if (Util::IsIgnoredUniform(kv.first))
				{
					continue;
				}
				hasProperty = true;
				PropertyRow(kv.first);
				ImGui::DragInt4("##int4", glm::value_ptr(kv.second));
				EndPropertyRow();
			}

			for (auto& kv : asset->GetFloats())
			{
				if (Util::IsIgnoredUniform(kv.first))
				{
					continue;
				}
				hasProperty = true;
				PropertyRow(kv.first);
				ImGui::DragFloat("##float", &kv.second, 0.01f);
				EndPropertyRow();
			}

			for (auto& kv : asset->GetFloat2s())
			{
				if (Util::IsIgnoredUniform(kv.first))
				{
					continue;
				}
				hasProperty = true;
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
				hasProperty = true;
				if (Util::ContainsIgnoreCase(kv.first, "color"))
				{
					DrawColorVec3(kv.first.c_str(), kv.second, 0.01f);
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
				hasProperty = true;
				if (Util::ContainsIgnoreCase(kv.first, "color"))
				{
					DrawColorVec4(kv.first.c_str(), kv.second, 0.01f);
					continue;
				}
				PropertyRow(kv.first);
				ImGui::DragFloat4("##float4", glm::value_ptr(kv.second), 0.01f);
				EndPropertyRow();
			}

			if (!hasProperty)
			{
				ImGui::TextDisabled("no property");
			}
		}

		ImGui::PopID();
	}

	void InspectorPanel::DrawAssetTexture(GUIContext& ctx)
	{
		auto asset = seri::asset::AssetManager::GetAssetByID<seri::TextureBase>(ctx.selectedAsset.id);
		if (!asset)
		{
			ImGui::TextDisabled("texture not loaded");
			return;
		}

		ScopedChild scopedChild("##AssetTexture", ImVec2(0, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY);

		ImGui::TextUnformatted("Texture");
		ImGui::Separator();

		DrawLabel("Size", fmt::format("{} x {}", asset->GetWidth(), asset->GetHeight()).c_str(), true);

		float size = ImGui::GetContentRegionAvail().x;
		if (size > 256.0f)
		{
			size = 256.0f;
		}

		ShowEditorImage(asset, size);
	}

	void InspectorPanel::DrawAssetFont(GUIContext& ctx)
	{
		auto asset = seri::asset::AssetManager::GetAssetByID<seri::font::Font>(ctx.selectedAsset.id);
		if (!asset)
		{
			ImGui::TextDisabled("font not loaded");
			return;
		}

		ScopedChild scopedChild("##AssetFont", ImVec2(0, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY);

		ImGui::TextUnformatted("Font");
		ImGui::Separator();

		const seri::font::FontMetrics& metrics = asset->GetMetrics();

		DrawLabel("Family", asset->GetFamilyName().c_str(), true);
		DrawLabel("Style", asset->GetStyleName().c_str(), true);
		DrawLabel("Baked Size", std::to_string(asset->GetDesc().pixelSize).c_str(), true);
		DrawLabel("Glyphs", std::to_string(asset->GetGlyphCount()).c_str(), true);
		DrawLabel("Line Height", fmt::format("{:.2f}", metrics.lineHeight).c_str(), true);

		auto atlas = asset->GetAtlas();
		if (!atlas)
		{
			return;
		}

		DrawLabel("Atlas", fmt::format("{} x {}", atlas->GetWidth(), atlas->GetHeight()).c_str(), true);

		float size = ImGui::GetContentRegionAvail().x;
		if (size > 256.0f)
		{
			size = 256.0f;
		}

		ShowEditorImage(atlas, size, /*flip*/ false);
	}

	void InspectorPanel::DrawAssetMesh(GUIContext& ctx)
	{
		auto asset = seri::asset::AssetManager::GetAssetByID<seri::Model>(ctx.selectedAsset.id);
		if (!asset)
		{
			ImGui::TextDisabled("mesh not loaded");
			return;
		}

		ScopedChild scopedChild("##AssetMesh", ImVec2(0, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY);

		ImGui::TextUnformatted("Mesh");
		ImGui::Separator();

		DrawLabel("Meshes", std::to_string(asset->meshes.size()).c_str(), true);
		DrawLabel("Materials", std::to_string(asset->materialCount).c_str(), true);

		float importScale = asset->importScale;
		if (DrawFloat("Scale", importScale, 0.01f, 0.0001f, 1000.0f, "%.4f"))
		{
			asset->SetImportScale(importScale);
		}

		ImGui::Spacing();

		for (size_t i = 0; i < asset->meshes.size(); i++)
		{
			auto& mesh = asset->meshes[i];

			ImGui::PushID(static_cast<int>(i));

			ImGui::AlignTextToFramePadding();
			ImGui::TextUnformatted(mesh->name.c_str());

			int materialIndex = mesh->materialIndex;
			if (DrawInt("Material", materialIndex, 0.1f, 0, asset->materialCount > 0 ? asset->materialCount - 1 : 0))
			{
				mesh->materialIndex = materialIndex;
			}

			if (!mesh->materialName.empty())
			{
				ImGui::TextDisabled("%s", mesh->materialName.c_str());
			}

			ImGui::PopID();
		}
	}

	void InspectorPanel::DrawAssetShader(GUIContext& ctx)
	{
		auto asset = seri::asset::AssetManager::GetAssetByID<seri::ShaderBase>(ctx.selectedAsset.id);
		if (!asset)
		{
			ImGui::TextDisabled("shader not loaded");
			return;
		}

		ScopedChild scopedChild("##AssetShader", ImVec2(0, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY);

		ImGui::TextUnformatted("Shader");
		ImGui::Separator();

		for (const auto& uniform : asset->GetUniforms())
		{
			ImGui::BulletText("%s : %s", uniform.name.c_str(), seri::UniformTypeToString(uniform.type));
		}
	}

	std::string InspectorPanel::GetMaterialSlotLabel(const std::shared_ptr<seri::Model>& model, size_t slot)
	{
		if (model)
		{
			for (const auto& mesh : model->meshes)
			{
				if (mesh->materialIndex == static_cast<int>(slot) && !mesh->materialName.empty())
				{
					return mesh->materialName;
				}
			}
		}

		return fmt::format("Element {}", slot);
	}

	bool InspectorPanel::DrawComponentHeader(const char* title)
	{
		bool remove = false;

		ImGui::PushID(title);

		ImGui::TextUnformatted(title);
		ImGui::SameLine();

		const char* removeStr = "X";

		float buttonWidth = ImGui::CalcTextSize(removeStr).x + ImGui::GetStyle().FramePadding.x * 2.0f;
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - buttonWidth);

		if (ImGui::SmallButton(removeStr))
		{
			remove = true;
		}

		ImGui::PopID();

		ImGui::Separator();

		return remove;
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

			if (ImGui::IsWindowAppearing())
			{
				search[0] = '\0';
				ImGui::SetKeyboardFocusHere();
			}

			ImGui::InputTextWithHint("##Search", "Search...", search, IM_ARRAYSIZE(search));

			ImGui::Spacing();

			for (const auto& info : seri::scene::SceneManager::GetCompIO())
			{
				if (info.name == seri::component::IDComponent::kCompName ||
					info.name == seri::component::TransformComponent::kCompName)
				{
					continue;
				}

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
				}

				ImGui::PopID();
			}

			ImGui::EndPopup();
		}
	}
}
