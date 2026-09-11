#include "gui/panels/HierarchyPanel.h"

namespace seri::editor
{
	void HierarchyPanel::Draw(GUIContext& ctx)
	{
		auto activeScene = seri::scene::SceneManager::GetActiveScene();
		std::string sceneName = fmt::format("{}{}", activeScene->GetName(), (activeScene->IsDirty() ? "*" : ""));
		seri::scene::SceneTreeNode& sceneTreeRoot = activeScene->GetSceneTreeRoot();

		static ImGuiTreeNodeFlags baseFlags =
			ImGuiTreeNodeFlags_OpenOnArrow |
			ImGuiTreeNodeFlags_SpanAvailWidth |
			ImGuiTreeNodeFlags_OpenOnDoubleClick;

		ImGuiTreeNodeFlags flags = baseFlags;
		if (ctx.selectedEntityId == activeScene->GetID())
		{
			flags |= ImGuiTreeNodeFlags_Selected;
		}

		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		bool open = ImGui::TreeNodeEx("##SceneRoot", flags, "%s", sceneName.c_str());

		bool clicked = ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen();
		if (clicked)
		{
			ctx.selectedEntityId = activeScene->GetID();
			ctx.inspectorType = InspectorType::scene;
		}

		if (ImGui::BeginPopupContextItem())
		{
			ShowAddMenu(activeScene, 0);
			ImGui::EndPopup();
		}

		if (open)
		{
			uint64_t selectedId = 0;
			DrawNode(ctx, activeScene, sceneTreeRoot, selectedId);
			if (!clicked && selectedId != 0)
			{
				ctx.selectedEntityId = selectedId;
				ctx.inspectorType = InspectorType::entity;
			}

			ImGui::TreePop();
		}

		if (ImGui::BeginPopupContextWindow("##HierarchyContext", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
		{
			ShowAddMenu(activeScene, 0);
			ImGui::EndPopup();
		}

		if (_pendingDeleteEntityId == 0 &&
			ctx.selectedEntityId != 0 &&
			!ImGui::GetIO().WantTextInput &&
			ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) &&
			ImGui::IsKeyPressed(ImGuiKey_Delete, false) &&
			activeScene->HasEntity(ctx.selectedEntityId))
		{
			_pendingDeleteEntityId = ctx.selectedEntityId;
		}

		if (_pendingDeleteEntityId != 0)
		{
			activeScene->DeleteEntity(_pendingDeleteEntityId);

			if (ctx.selectedEntityId != 0 && !activeScene->HasEntity(ctx.selectedEntityId))
			{
				ctx.selectedEntityId = 0;
				ctx.inspectorType = InspectorType::none;
			}

			_pendingDeleteEntityId = 0;
		}
	}

	void HierarchyPanel::DrawNode(GUIContext& ctx, const std::shared_ptr<seri::scene::Scene>& activeScene, seri::scene::SceneTreeNode& node, uint64_t& selectedId)
	{
		static ImGuiTreeNodeFlags baseFlags =
			ImGuiTreeNodeFlags_OpenOnArrow |
			ImGuiTreeNodeFlags_SpanAvailWidth |
			ImGuiTreeNodeFlags_OpenOnDoubleClick;

		for (auto& child : node.children)
		{
			ImGuiTreeNodeFlags flags = baseFlags;

			if (child.children.empty())
			{
				flags |= ImGuiTreeNodeFlags_Leaf;
			}
			if (ctx.selectedEntityId == child.id)
			{
				flags |= ImGuiTreeNodeFlags_Selected;
			}

			entt::entity entity = activeScene->GetEntityByID(child.id);
			if (entity == entt::null)
			{
				continue;
			}

			if (_pendingExpandEntityId == child.id)
			{
				ImGui::SetNextItemOpen(true);
				_pendingExpandEntityId = 0;
			}

			auto* idComponent = seri::scene::SceneManager::GetRegistry().try_get<seri::component::IDComponent>(entity);
			if (!idComponent)
			{
				continue;
			}

			std::string label = idComponent->name;

			bool open = ImGui::TreeNodeEx((void*)(intptr_t)child.id, flags, "%s", label.c_str());

			if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
			{
				selectedId = child.id;
			}

			if (ImGui::BeginPopupContextItem())
			{
				ShowAddMenu(activeScene, child.id);

				if (ImGui::MenuItem("Delete"))
				{
					_pendingDeleteEntityId = child.id;
				}

				ImGui::EndPopup();
			}

			if (open)
			{
				DrawNode(ctx, activeScene, child, selectedId);

				ImGui::TreePop();
			}
		}
	}

	void HierarchyPanel::ShowAddMenu(const std::shared_ptr<seri::scene::Scene>& activeScene, uint64_t parentId)
	{
		if (ImGui::BeginMenu("Add"))
		{
			if (ImGui::MenuItem("Empty Object"))
			{
				AddEntity(activeScene, parentId, "Entity", {});
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Camera"))
			{
				AddEntity(activeScene, parentId, "Camera", { seri::component::CameraComponent::kCompName });
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Directional Light"))
			{
				AddEntity(activeScene, parentId, "Directional Light", { seri::component::DirectionalLightComponent::kCompName });
			}
			if (ImGui::MenuItem("Spot Light"))
			{
				AddEntity(activeScene, parentId, "Spot Light", { seri::component::SpotLightComponent::kCompName });
			}
			if (ImGui::MenuItem("Point Light"))
			{
				AddEntity(activeScene, parentId, "Point Light", { seri::component::PointLightComponent::kCompName });
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Mesh"))
			{
				AddEntity(activeScene, parentId, "Mesh", {
					seri::component::MeshComponent::kCompName,
					seri::component::MeshRendererComponent::kCompName,
					});
			}
			if (ImGui::MenuItem("Skinned Mesh"))
			{
				AddEntity(activeScene, parentId, "Skinned Mesh", {
					seri::component::SkinnedMeshRendererComponent::kCompName,
					seri::component::AnimatorComponent::kCompName,
					});
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Sprite"))
			{
				AddEntity(activeScene, parentId, "Sprite", { seri::component::SpriteRendererComponent::kCompName });
			}
			if (ImGui::MenuItem("Text"))
			{
				AddEntity(activeScene, parentId, "Text", { seri::component::TextComponent::kCompName });
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Audio"))
			{
				AddEntity(activeScene, parentId, "Audio", { seri::component::AudioComponent::kCompName });
			}
			if (ImGui::MenuItem("Script"))
			{
				AddEntity(activeScene, parentId, "Script", { seri::component::ScriptComponent::kCompName });
			}

			ImGui::EndMenu();
		}
	}

	void HierarchyPanel::AddEntity(const std::shared_ptr<seri::scene::Scene>& activeScene, uint64_t parentId, const char* name, std::initializer_list<std::string_view> compNames)
	{
		uint64_t entityId = seri::Random::UUID();
		activeScene->AddEntityAsChild(entityId, parentId, name);

		_pendingExpandEntityId = parentId;

		entt::entity entity = activeScene->GetEntityByID(entityId);
		for (std::string_view compName : compNames)
		{
			seri::scene::SceneManager::AddComponent(entity, compName);
		}
	}
}
