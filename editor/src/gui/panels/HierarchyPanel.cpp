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
				activeScene->AddEntityAsChild(seri::Random::UUID(), parentId, "Entity");

				_pendingExpandEntityId = parentId;
			}
			if (ImGui::MenuItem("Camera"))
			{
				uint64_t entityId = seri::Random::UUID();
				activeScene->AddEntityAsChild(entityId, parentId, "Camera");

				_pendingExpandEntityId = parentId;

				entt::entity entity = activeScene->GetEntityByID(entityId);
				seri::scene::SceneManager::AddComponent(entity, seri::component::CameraComponent::compName);
			}
			if (ImGui::MenuItem("Plane"))
			{
				uint64_t entityId = seri::Random::UUID();
				activeScene->AddEntityAsChild(entityId, parentId, "Plane");

				_pendingExpandEntityId = parentId;

				entt::entity entity = activeScene->GetEntityByID(entityId);
				auto& registry = seri::scene::SceneManager::GetRegistry();

				// TODO: use default assets instead of creating new ones every time and handle serialization

				auto model = std::make_shared<seri::Model>();
				model->id = seri::Random::UUID();
				model->materialCount = 1;
				model->meshes.push_back(std::move(seri::Mesh::plane_3d(128, 8.0f)));
				model->Build();
				seri::asset::AssetManager::AddAsset(model->id, model);

				const int comp = 4;
				const int dimX = 256;
				const int dimY = 256;
				const int totalBytes = dimX * dimY * comp;

				uint8_t* white = (uint8_t*)malloc(totalBytes);
				uint8_t* normal = (uint8_t*)malloc(totalBytes);
				uint8_t* arm = (uint8_t*)malloc(totalBytes);

				for (int i = 0; i < totalBytes; i += comp)
				{
					white[i + 0] = 255;
					white[i + 1] = 255;
					white[i + 2] = 255;
					white[i + 3] = 255;

					normal[i + 0] = 128; // x = 0
					normal[i + 1] = 128; // y = 0
					normal[i + 2] = 255; // z = 1
					normal[i + 3] = 255;

					arm[i + 0] = 255; // AO = 1.0
					arm[i + 1] = 128; // Roughness = 0.5
					arm[i + 2] = 0;   // Metallic = 0.0
					arm[i + 3] = 255;
				}

				auto diffTex = seri::TextureBase::Create();
				diffTex->id = seri::Random::UUID();
				diffTex->Init(seri::TextureDesc{}, white, dimX, dimY, comp);
				auto normalTex = seri::TextureBase::Create();
				normalTex->id = seri::Random::UUID();
				normalTex->Init(seri::TextureDesc{}, normal, dimX, dimY, comp);
				auto armTex = seri::TextureBase::Create();
				armTex->id = seri::Random::UUID();
				armTex->Init(seri::TextureDesc{}, arm, dimX, dimY, comp);

				auto material = std::make_shared<seri::Material>();
				material->id = seri::Random::UUID();
				material->SetShader(seri::ShaderLibrary::Find("pbr"));
				material->SetTexture(seri::literals::kUniformDiffTexture, diffTex);
				material->SetTexture(seri::literals::kUniformNormalTexture, normalTex);
				material->SetTexture(seri::literals::kUniformArmTexture, armTex);
				seri::asset::AssetManager::AddAsset(material->id, material);

				registry.emplace_or_replace<seri::component::MeshComponent>(entity, seri::component::MeshComponent{ model->id });
				registry.emplace_or_replace<seri::component::MeshRendererComponent>(entity, seri::component::MeshRendererComponent{ material->id });
			}

			ImGui::EndMenu();
		}
	}
}
