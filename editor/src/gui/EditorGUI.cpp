#include "EditorGUI.h"

namespace seri::editor
{
	EditorGUI::EditorGUI()
	{
		LOGGER(info, "[gui] editor init succeeded");
	}

	EditorGUI::~EditorGUI()
	{
		ImGui_ImplOpenGL3_Shutdown();

#if defined (SERI_USE_WINDOW_GLFW)
		ImGui_ImplGlfw_Shutdown();
#elif defined (SERI_USE_WINDOW_SDL3)
		ImGui_ImplSDL3_Shutdown();
#endif

		ImGui::DestroyContext();

		LOGGER(info, "[gui] editor delete succeeded");
	}

	void EditorGUI::Init()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		SetIO();
		SetStyle();

#if defined (SERI_USE_WINDOW_GLFW)
		ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(seri::WindowManager::GetWindowHandle()), true);
#elif defined (SERI_USE_WINDOW_SDL3)
		ImGui_ImplSDL3_InitForOpenGL(static_cast<SDL_Window*>(seri::WindowManager::GetWindowHandle()), seri::WindowManager::GetContext());
#endif

		ImGui_ImplOpenGL3_Init("#version 460");
	}

	void EditorGUI::Update()
	{
		ImGui_ImplOpenGL3_NewFrame();
#if defined (SERI_USE_WINDOW_GLFW)
		ImGui_ImplGlfw_NewFrame();
#elif defined (SERI_USE_WINDOW_SDL3)
		ImGui_ImplSDL3_NewFrame();
#endif

		ImGui::NewFrame();

		ImGuizmo::BeginFrame();
		ImGuizmo::Enable(true);

		DrawEditorLayout();

		CheckShortcuts();
	}

	void EditorGUI::Render()
	{
		ImGui::Render();

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

#if defined (SERI_USE_WINDOW_GLFW)
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
#elif defined (SERI_USE_WINDOW_SDL3)
		SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
		SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
#endif
	}

	void EditorGUI::ProcessEvent(const void* event)
	{
#if defined (SERI_USE_WINDOW_SDL3)
		ImGui_ImplSDL3_ProcessEvent(static_cast<const SDL_Event*>(event));
#endif
	}

	void EditorGUI::CheckShortcuts()
	{
		auto& io = ImGui::GetIO();

		if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_S))
		{
			seri::scene::SceneManager::GetActiveScene()->Save();
			seri::asset::AssetManager::GetInstance().Save();
		}
	}

	void EditorGUI::ShowEditorMainMenuBar()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Noop", "Noop"))
				{
				}

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("Save", "CTRL+S"))
				{
					seri::scene::SceneManager::GetActiveScene()->Save();
				}

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}

	void EditorGUI::ShowEditorSceneImage()
	{
		ImVec2 panelSize = ImGui::GetContentRegionAvail();

		float fbAspect = seri::RenderingManager::GetEditorRT()->GetAspectRatio();
		float panelAspect = panelSize.x / panelSize.y;

		ImVec2 imageSize;
		if (panelAspect > fbAspect)
		{
			imageSize.y = panelSize.y;
			imageSize.x = panelSize.y * fbAspect;
		}
		else
		{
			imageSize.x = panelSize.x;
			imageSize.y = panelSize.x / fbAspect;
		}

		ImVec2 cursorPos = ImGui::GetCursorScreenPos();

		float offsetX = (panelSize.x - imageSize.x) * 0.5f;
		float offsetY = (panelSize.y - imageSize.y) * 0.5f;

		ImVec2 drawPos = ImVec2(cursorPos.x + offsetX, cursorPos.y + offsetY);
		ImVec2 imageMin = drawPos;
		ImVec2 imageMax = ImVec2(drawPos.x + imageSize.x, drawPos.y + imageSize.y);

		auto tex = (ImTextureID)(intptr_t)seri::RenderingManager::GetEditorRT()->GetFirstColorTextureHandle();

		ImGui::GetWindowDrawList()->AddImage(
			tex,
			imageMin,
			imageMax,
			ImVec2(0, 1),
			ImVec2(1, 0)
		);

		ShowEditorSceneGizmo(imageMin, imageSize);
		ShowEditorSceneEntityGizmo(imageMin, imageSize);
		ShowEditorSceneGizmoToolbar(imageMin);
		ControlEditorSceneMove(imageMin, imageMax);
	}

	void EditorGUI::ControlEditorSceneMove(const ImVec2& imageMin, const ImVec2& imageMax)
	{
		static bool moveActive = false;
		static ImVec2 lastMouse = ImVec2(0, 0);

		ImVec2 mouse = ImGui::GetMousePos();

		if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		{
			bool panelActive =
				ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) ||
				ImGui::IsWindowFocused();
			bool mouseInside =
				mouse.x >= imageMin.x && mouse.x <= imageMax.x &&
				mouse.y >= imageMin.y && mouse.y <= imageMax.y;

			if (panelActive && mouseInside)
			{
				moveActive = true;
				lastMouse = mouse;
			}
		}
		if (ImGui::IsMouseReleased(ImGuiMouseButton_Right))
		{
			moveActive = false;
		}

		if (moveActive)
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_None);

			ImVec2 delta = { mouse.x - lastMouse.x, mouse.y - lastMouse.y };
			lastMouse = mouse;

			auto deltaTime = seri::TimeWrapper::GetDeltaTime();

			auto camera = seri::Graphic::GetCameraPerspective();
			auto& camProps = camera->GetCameraProperties();

			float sensitivity = camProps.sensitivity;

			float yawDeltaDeg = -delta.x * sensitivity;
			float pitchDeltaDeg = -delta.y * sensitivity;

			glm::vec3 front = camera->GetFront();
			glm::vec3 right = camera->GetRight();
			glm::vec3 worldUp{ 0.0f, 1.0f, 0.0f };

			glm::quat qYaw = glm::angleAxis(glm::radians(yawDeltaDeg), worldUp);
			glm::quat orientAfterYaw = glm::normalize(qYaw * camProps.rotation);
			glm::vec3 rightAfterYaw = glm::normalize(orientAfterYaw * glm::vec3(1.0f, 0.0f, 0.0f));
			glm::quat qPitch = glm::angleAxis(glm::radians(pitchDeltaDeg), rightAfterYaw);
			camProps.rotation = glm::normalize(qPitch * orientAfterYaw);

			float speed = ImGui::GetIO().KeyShift ? camProps.speed * 2.0f : camProps.speed;

			if (ImGui::IsKeyDown(ImGuiKey_W))
			{
				camProps.position += front * speed * deltaTime;
			}
			if (ImGui::IsKeyDown(ImGuiKey_S))
			{
				camProps.position -= front * speed * deltaTime;
			}
			if (ImGui::IsKeyDown(ImGuiKey_A))
			{
				camProps.position -= right * speed * deltaTime;
			}
			if (ImGui::IsKeyDown(ImGuiKey_D))
			{
				camProps.position += right * speed * deltaTime;
			}
		}
	}

	void EditorGUI::ShowEditorSceneGizmoToolbar(const ImVec2& imageMin)
	{
		ImGui::SetNextWindowPos(ImVec2(imageMin.x + 5.0f, imageMin.y + 5.0f));
		ImGui::SetNextWindowBgAlpha(0.85f);

		ImGuiWindowFlags flags =
			ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_NoDocking |
			ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoFocusOnAppearing |
			ImGuiWindowFlags_NoNav;

		ImGui::Begin("##SceneGizmoToolbar", nullptr, flags);

		ImGui::TextUnformatted("Gizmo");
		ImGui::Separator();

		int space = static_cast<int>(_gizmoSpace);
		ImGui::RadioButton("Local", &space, static_cast<int>(GizmoSpace::local));
		ImGui::RadioButton("World", &space, static_cast<int>(GizmoSpace::world));
		_gizmoSpace = static_cast<GizmoSpace>(space);

		ImGui::Separator();

		int op = static_cast<int>(_gizmoOperation);
		ImGui::RadioButton("Translate", &op, static_cast<int>(GizmoOperation::translate));
		ImGui::RadioButton("Rotate", &op, static_cast<int>(GizmoOperation::rotate));
		ImGui::RadioButton("Scale", &op, static_cast<int>(GizmoOperation::scale));
		_gizmoOperation = static_cast<GizmoOperation>(op);

		ImGui::End();
	}

	void EditorGUI::ShowEditorSceneGizmo(const ImVec2& imageMin, const ImVec2& imageSize)
	{
		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(
			imageMin.x,
			imageMin.y,
			imageSize.x,
			imageSize.y
		);

		float gizmoSize = 64.0f;
		float padding = 5.0f;
		bool alwaysRun = true;

		float x = imageMin.x + imageSize.x - gizmoSize - padding;
		float y = imageMin.y + padding;

		auto camera = seri::Graphic::GetCameraPerspective();

		glm::mat4 view = camera->GetView();

		ImGuizmo::PushID("scene_gizmo");

		ImGuizmo::ViewManipulate(
			glm::value_ptr(view),
			8.0f,
			ImVec2(x, y),
			ImVec2(gizmoSize, gizmoSize),
			0x10101010
		);

		if (ImGuizmo::IsUsing() || alwaysRun)
		{
			camera->SetFromViewMatrix(view);
		}

		ImGuizmo::PopID();
	}

	void EditorGUI::ShowEditorSceneEntityGizmo(const ImVec2& imageMin, const ImVec2& imageSize)
	{
		if (_inspectorType != InspectorType::entity || _selectedEntityId == 0)
		{
			return;
		}

		ImGuizmo::MODE mode = ImGuizmo::WORLD;
		switch (_gizmoSpace)
		{
			case seri::editor::EditorGUI::GizmoSpace::local:
				mode = ImGuizmo::LOCAL;
				break;
			case seri::editor::EditorGUI::GizmoSpace::world:
				mode = ImGuizmo::WORLD;
				break;
		}

		ImGuizmo::OPERATION operation = ImGuizmo::TRANSLATE;
		switch (_gizmoOperation)
		{
			case seri::editor::EditorGUI::GizmoOperation::translate:
				operation = ImGuizmo::TRANSLATE;
				break;
			case seri::editor::EditorGUI::GizmoOperation::rotate:
				operation = ImGuizmo::ROTATE;
				break;
			case seri::editor::EditorGUI::GizmoOperation::scale:
				operation = ImGuizmo::SCALE;
				break;
		}

		if (_gizmoOperation == GizmoOperation::rotate)
		{
			mode = ImGuizmo::LOCAL;
			_gizmoSpace = GizmoSpace::local;
		}

		auto scene = seri::scene::SceneManager::GetActiveScene();
		auto& registry = seri::scene::SceneManager::GetRegistry();
		auto entity = scene->GetEntityByID(_selectedEntityId);

		auto* idComp = registry.try_get<seri::component::IDComponent>(entity);
		auto* transformComp = registry.try_get<seri::component::TransformComponent>(entity);
		if (idComp == nullptr || transformComp == nullptr)
		{
			return;
		}

		glm::mat4 parentWorld{ 1.0f };
		if (idComp->parentId != 0)
		{
			auto parentEntity = scene->GetEntityByID(idComp->parentId);
			auto* parentTransformComp = registry.try_get<seri::component::TransformComponent>(parentEntity);
			parentWorld = parentTransformComp->worldMatrix;
		}

		glm::mat4 worldMatrix = parentWorld * transformComp->localMatrix;
		glm::vec3 lockedWorldPos = glm::vec3(worldMatrix[3]);

		ImGuizmo::PushID("scene_entity_gizmo");

		ImGuizmo::Manipulate(
			glm::value_ptr(Graphic::GetCameraPerspective()->GetView()),
			glm::value_ptr(Graphic::GetCameraPerspective()->GetProjection()),
			operation,
			mode,
			glm::value_ptr(worldMatrix),
			nullptr
		);

		if (ImGuizmo::IsUsing())
		{
			if (_gizmoOperation == GizmoOperation::rotate)
			{
				worldMatrix[3] = glm::vec4(lockedWorldPos, 1.0f);
			}

			Util::Decompose(
				glm::inverse(parentWorld) * worldMatrix,
				transformComp->position,
				transformComp->rotation,
				transformComp->scale
			);

			scene->SetAsDirty();
		}

		ImGuizmo::PopID();
	}

	void EditorGUI::ShowEditorHierarchy()
	{
		auto activeScene = seri::scene::SceneManager::GetActiveScene();
		std::string sceneName = fmt::format("{}{}", activeScene->GetName(), (activeScene->IsDirty() ? "*" : ""));
		seri::scene::SceneTreeNode& sceneTreeRoot = activeScene->GetSceneTreeRoot();

		static ImGuiTreeNodeFlags baseFlags =
			ImGuiTreeNodeFlags_OpenOnArrow |
			ImGuiTreeNodeFlags_SpanAvailWidth |
			ImGuiTreeNodeFlags_OpenOnDoubleClick;

		ImGuiTreeNodeFlags flags = baseFlags;
		if (_selectedEntityId == activeScene->GetID())
		{
			flags |= ImGuiTreeNodeFlags_Selected;
		}

		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (ImGui::TreeNodeEx(sceneName.c_str(), flags))
		{
			bool clicked = ImGui::IsItemClicked();
			if (clicked)
			{
				_selectedEntityId = activeScene->GetID();
				_inspectorType = InspectorType::scene;
			}

			if (ImGui::BeginPopupContextItem())
			{
				if (ImGui::BeginMenu("Add"))
				{
					if (ImGui::MenuItem("Empty Object"))
					{
						activeScene->AddEntityAsChild(seri::Random::UUID(), 0, "Entity");
					}
					ImGui::EndMenu();
				}

				ImGui::EndPopup();
			}

			uint64_t selectedId = 0;
			ShowEditorHierarchyImpl(activeScene, sceneTreeRoot, selectedId);
			if (!clicked && selectedId != 0)
			{
				_selectedEntityId = selectedId;
				_inspectorType = InspectorType::entity;
			}

			ImGui::TreePop();
		}
	}

	void EditorGUI::ShowEditorHierarchyImpl(const std::shared_ptr<seri::scene::Scene>& activeScene, seri::scene::SceneTreeNode& node, uint64_t& selectedId)
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
			if (_selectedEntityId == child.id)
			{
				flags |= ImGuiTreeNodeFlags_Selected;
			}

			entt::entity entity = activeScene->GetEntityByID(child.id);
			if (entity == entt::null)
			{
				continue;
			}

			auto* idComponent = seri::scene::SceneManager::GetRegistry().try_get<seri::component::IDComponent>(entity);
			if (!idComponent)
			{
				continue;
			}

			std::string label = idComponent->name;

			bool open = ImGui::TreeNodeEx((void*)(intptr_t)child.id, flags, "%s", label.c_str());
			bool clicked = ImGui::IsItemClicked();

			if (clicked)
			{
				selectedId = child.id;
			}

			if (ImGui::BeginPopupContextItem())
			{
				if (ImGui::BeginMenu("Add"))
				{
					if (ImGui::MenuItem("Empty Object"))
					{
						seri::scene::SceneManager::GetActiveScene()->AddEntityAsChild(seri::Random::UUID(), child.id, "Entity");
					}
					ImGui::EndMenu();
				}

				if (ImGui::MenuItem("Delete"))
				{
					seri::scene::SceneManager::GetActiveScene()->DeleteEntity(child.id);
				}

				ImGui::EndPopup();
			}

			if (open)
			{
				ShowEditorHierarchyImpl(activeScene, child, selectedId);

				ImGui::TreePop();
			}
		}
	}

	void EditorGUI::ShowEditorInspector()
	{
		if (_selectedEntityId == 0)
		{
			ImGui::Text("no entity selected");
			return;
		}

		if (_inspectorType == InspectorType::none)
		{
			ImGui::Text("no inspector type selected");
			return;
		}

		switch (_inspectorType)
		{
			case seri::editor::EditorGUI::InspectorType::scene:
				{
					ShowEditorInspectorScene();
				}
				break;
			case seri::editor::EditorGUI::InspectorType::entity:
				{
					ShowEditorInspectorEntity();
				}
				break;
			case seri::editor::EditorGUI::InspectorType::asset:
				{
					ShowEditorInspectorAsset();
				}
				break;
			default:
				{
					LOGGER(error, "[gui] editor unknown inspector type");
				}
				break;
		}
	}

	void EditorGUI::ShowEditorInspectorScene()
	{
		auto scene = seri::scene::SceneManager::GetActiveScene();

		auto idComponent = scene->GetIDComponent();
		{
			ImGui::Text(fmt::format("id: {}", idComponent.id).c_str());
			ImGui::Text(fmt::format("parent id: {}", idComponent.parentId).c_str());
			ImGui::Text(fmt::format("name: {}", idComponent.name.c_str()).c_str());

			ImGui::Separator();
		}

		auto sceneComponent = scene->GetSceneComponent();
		{
			ImGui::Text(fmt::format("version: {}", sceneComponent.version).c_str());
			ImGui::Text(fmt::format("active: {}", sceneComponent.isActive).c_str());

			ImGui::Separator();
		}
	}

	void EditorGUI::ShowEditorInspectorEntity()
	{
		auto scene = seri::scene::SceneManager::GetActiveScene();
		auto& registry = seri::scene::SceneManager::GetRegistry();

		auto entity = scene->GetEntityByID(_selectedEntityId);

		ImGuiChildFlags childFlags =
			ImGuiChildFlags_Borders |
			ImGuiChildFlags_AutoResizeY;

		auto DrawVec3 = [](const char* label, glm::vec3& v, float speed)
			{
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
					ImGui::DragFloat("##v", &v[i], speed);
					ImGui::PopID();

					if (i < 2)
					{
						ImGui::SameLine();
					}
				}

				ImGui::Columns(1);
				ImGui::PopID();
			};

		if (auto* idComp = registry.try_get<seri::component::IDComponent>(entity))
		{
			if (ImGui::BeginChild("##IDComponent", ImVec2(0, 0), childFlags))
			{
				ImGui::TextUnformatted("Entity");
				ImGui::Separator();

				ImGui::Columns(2, nullptr, false);
				ImGui::SetColumnWidth(0, 90.0f);

				ImGui::AlignTextToFramePadding();
				ImGui::TextUnformatted("Name");
				ImGui::NextColumn();

				char buffer[256]{};
				strncpy_s(buffer, idComp->name.c_str(), sizeof(buffer) - 1);
				ImGui::SetNextItemWidth(-1);
				if (ImGui::InputText("##EntityName", buffer, sizeof(buffer)))
				{
					idComp->name = buffer;
					scene->SetAsDirty();
				}

				ImGui::NextColumn();

				ImGui::AlignTextToFramePadding();
				ImGui::TextUnformatted("ID");
				ImGui::NextColumn();

				ImGui::TextDisabled("%llu", idComp->id);

				ImGui::NextColumn();

				ImGui::AlignTextToFramePadding();
				ImGui::TextUnformatted("Parent");
				ImGui::NextColumn();

				if (idComp->parentId != 0)
				{
					ImGui::TextDisabled("%llu", idComp->parentId);
				}
				else
				{
					ImGui::TextDisabled("<None>");
				}

				ImGui::Columns(1);

				ImGui::EndChild();
			}
		}

		if (auto* transformComp = registry.try_get<seri::component::TransformComponent>(entity))
		{
			if (ImGui::BeginChild("##TransformComponent", ImVec2(0, 0), childFlags))
			{
				ImGui::TextUnformatted("Transform Component");
				ImGui::Separator();

				DrawVec3("Position", transformComp->position, 0.1f);
				DrawVec3("Rotation", transformComp->rotation, 0.5f);
				DrawVec3("Scale", transformComp->scale, 0.1f);

				ImGui::EndChild();
			}
		}

		if (auto* meshComp = registry.try_get<seri::component::MeshComponent>(entity))
		{
			if (ImGui::BeginChild("##MeshComponent", ImVec2(0, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY))
			{
				ImGui::TextUnformatted("Mesh Component");
				ImGui::Separator();

				ImGui::Columns(2, nullptr, false);
				ImGui::SetColumnWidth(0, 90.0f);

				ImGui::AlignTextToFramePadding();
				ImGui::TextUnformatted("Mesh");
				ImGui::NextColumn();

				std::string assetName = meshComp->meshAssetId != 0 ? seri::asset::AssetManager::GetAssetName(meshComp->meshAssetId) : "<None>";

				ImGui::SetNextItemWidth(-1);

				if (ImGui::Button(assetName.c_str(), ImVec2(-1, 0)))
				{
					ImGui::OpenPopup("AssetPickerPopup");
				}

				bool selected = false;
				uint64_t selection = 0;
				if (ShowEditorAssetPickerPopup(seri::asset::AssetType::mesh, selected, selection))
				{
					if (selected)
					{
						scene->SetAsDirty();
						meshComp->meshAssetId = selection;
					}
				}

				ImGui::Columns(1);

				ImGui::EndChild();
			}
		}

		if (auto* meshRendererComp = registry.try_get<seri::component::MeshRendererComponent>(entity))
		{
			if (ImGui::BeginChild("##MeshRendererComponent", ImVec2(0, 0), childFlags))
			{
				ImGui::TextUnformatted("Mesh Renderer Component");
				ImGui::Separator();

				ImGui::Columns(2, nullptr, false);
				ImGui::SetColumnWidth(0, 90.0f);

				//ImGui::AlignTextToFramePadding();
				//ImGui::TextUnformatted("Cast Shadow");
				//ImGui::NextColumn();
				//if (ImGui::Checkbox("##CastShadow", &meshRendererComp->castShadow))
				//{
				//	scene->SetAsDirty();
				//}
				//ImGui::NextColumn();

				ImGui::AlignTextToFramePadding();
				ImGui::TextUnformatted("Material");
				ImGui::NextColumn();

				std::string materialName = meshRendererComp->materialAssetId != 0 ? seri::asset::AssetManager::GetAssetName(meshRendererComp->materialAssetId) : "<None>";
				if (ImGui::Button(materialName.c_str(), ImVec2(-1, 0)))
				{
					ImGui::OpenPopup("AssetPickerPopup");
				}

				bool selected = false;
				uint64_t selection = 0;
				if (ShowEditorAssetPickerPopup(seri::asset::AssetType::material, selected, selection))
				{
					if (selected)
					{
						scene->SetAsDirty();
						meshRendererComp->materialAssetId = selection;
					}
				}

				ImGui::Columns(1);

				ImGui::EndChild();
			}
		}

		ImGui::NewLine();

		if (ImGui::Button("Add Component", ImVec2(-1, 0)))
		{
			ImGui::OpenPopup("AddComponentPopup");
		}
		ShowEditorComponentPickerPopup();
	}

	void EditorGUI::ShowEditorInspectorAsset()
	{
		switch (_selectedAsset.type)
		{
			case seri::asset::AssetType::material:
				{
					ShowEditorInspectorAssetMaterial();
				}
				break;
			case seri::asset::AssetType::shader:
				{
					auto asset = seri::asset::AssetManager::GetAssetByID<seri::ShaderBase>(_selectedAsset.id);
					ImGui::Text(fmt::format("shader: {}", asset->id).c_str());
				}
				break;
			case seri::asset::AssetType::texture:
				{
					auto asset = seri::asset::AssetManager::GetAssetByID<seri::TextureBase>(_selectedAsset.id);
					ImGui::Text(fmt::format("texture: {}", asset->id).c_str());
				}
				break;
			case seri::asset::AssetType::mesh:
				{
					auto asset = seri::asset::AssetManager::GetAssetByID<seri::Model>(_selectedAsset.id);
					ImGui::Text(fmt::format("mesh: {}", asset->id).c_str());
					ImGui::Text(fmt::format("material count: {}", asset->materialCount).c_str());
					for (auto& mesh : asset->meshes)
					{
						ImGui::Text(fmt::format(" mesh: {}, mat: {}", mesh->name, mesh->materialIndex).c_str());
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

	void EditorGUI::ShowEditorInspectorAssetMaterial()
	{
		constexpr float previewSize = 64.0f;
		constexpr float labelWidth = 150.0f;

		auto asset = seri::asset::AssetManager::GetAssetByID<seri::Material>(_selectedAsset.id);
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
		ImGui::TextDisabled("Shader: %llu", asset->GetShader()->id);

		{
			ImGui::Separator();

			PropertyRow("Shader");

			std::string shaderName = asset->GetShader()->id == 0 ? "<None>" : seri::asset::AssetManager::GetAssetName(asset->GetShader()->id);
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

	void EditorGUI::ShowEditorComponentPickerPopup()
	{
		static char search[128] = "";

		auto scene = seri::scene::SceneManager::GetActiveScene();
		auto entity = scene->GetEntityByID(_selectedEntityId);

		if (ImGui::BeginPopup("AddComponentPopup"))
		{
			ImGui::TextUnformatted("Add Component");
			ImGui::Separator();

			ImGui::InputTextWithHint("##Search", "Search components...", search, IM_ARRAYSIZE(search));

			ImGui::Spacing();

			for (const auto& info : seri::scene::SceneManager::GetCompIO())
			{
				if (search[0] != '\0' && info.name.find(search) == std::string::npos)
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

	bool EditorGUI::ShowEditorAssetPickerPopup(seri::asset::AssetType type, bool& selected, uint64_t& selection)
	{
		selected = false;
		selection = 0;

		if (!ImGui::BeginPopup("AssetPickerPopup"))
		{
			return false;
		}

		static char search[64]{};
		const float size = 64.0f;

		ImGui::InputText("Search", search, sizeof(search));
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

		const auto MatchesSearch = [](std::string_view name, std::string_view search) -> bool
			{
				if (search.empty())
				{
					return true;
				}
				auto it = std::search(name.begin(), name.end(), search.begin(), search.end(),
					[](char a, char b)
					{
						return std::tolower(a) == std::tolower(b);
					}
				);
				return it != name.end();
			};

		seri::asset::AssetMetadata noneAsset{
			.id = 0,
			.name = "<none>"
		};
		auto assetMedataList = seri::asset::AssetManager::GetAssetsByType(type);
		assetMedataList.emplace_back(noneAsset);

		for (auto assetMetadata : assetMedataList)
		{
			if (!MatchesSearch(assetMetadata.name, search))
			{
				continue;
			}

			ImGui::PushID(assetMetadata.id);

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
							search[0] = '\0';
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
							search[0] = '\0';
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

	void EditorGUI::ShowEditorImage(std::shared_ptr<seri::TextureBase>& texture, float size)
	{
		auto tex = (ImTextureID)(intptr_t)texture->GetHandle();
		ImGui::Image(tex, { size, size }, { 0, 1 }, { 1, 0 });
	}

	bool EditorGUI::ShowEditorImageButton(std::shared_ptr<seri::TextureBase>& texture, float size)
	{
		auto tex = (ImTextureID)(intptr_t)texture->GetHandle();
		return ImGui::ImageButton(fmt::format("tex##{}", texture->id).c_str(), tex, { size, size }, { 0, 1 }, { 1, 0 });
	}

	void EditorGUI::ShowEditorProject()
	{
		seri::asset::AssetTreeNode& assetTreeRoot = seri::asset::AssetManager::GetAssetTreeRoot();

		ImGui::Columns(2);

		ImGui::BeginChild("Folders", ImVec2(0, 0), true);
		ShowEditorProjectFolderTree(assetTreeRoot);
		ImGui::EndChild();

		ImGui::NextColumn();

		ImGui::BeginChild("Assets", ImVec2(0, 0), true);
		ShowEditorProjectAssetGrid();
		ImGui::EndChild();

		ImGui::Columns(1);
	}

	void EditorGUI::ShowEditorProjectFolderTree(seri::asset::AssetTreeNode& node)
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

		if (ImGui::TreeNodeEx((void*)(intptr_t)node.id, flags, "%s", label.c_str()))
		{
			if (ImGui::IsItemClicked())
			{
				_selectedFolder = &node;
			}

			for (auto& child : node.children)
			{
				if (child.isFolder)
				{
					ShowEditorProjectFolderTree(child);
				}
			}

			ImGui::TreePop();
		}
	}

	void EditorGUI::ShowEditorProjectAssetGrid()
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
				_selectedAsset = child;
				_selectedEntityId = child.id;
				_inspectorType = InspectorType::asset;
			}

			ImGui::EndGroup();
			ImGui::NextColumn();
		}

		ImGui::Columns(1);
	}

	void EditorGUI::DrawEditorLayout()
	{
		static bool showMainMenu = true;
		static bool showHierarchy = true;
		static bool showScene = true;
		static bool showGame = true;
		static bool showInspector = true;
		static bool showConsole = true;
		static bool showProject = true;

		ImGuiWindowFlags window_flags =
			ImGuiWindowFlags_MenuBar |
			ImGuiWindowFlags_NoDocking
			;

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);

		ImGui::Begin("Editor DockSpace", nullptr,
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoNavFocus
		);

		ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0, 0), ImGuiDockNodeFlags_None);

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Window"))
			{
				ImGui::MenuItem("Hierarchy", nullptr, &showHierarchy);
				ImGui::MenuItem("Scene", nullptr, &showScene);
				ImGui::MenuItem("Game", nullptr, &showGame);
				ImGui::MenuItem("Inspector", nullptr, &showInspector);
				ImGui::MenuItem("Project", nullptr, &showProject);
				ImGui::MenuItem("Console", nullptr, &showConsole);

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		ImGui::End();

		if (showMainMenu)
		{
			ShowEditorMainMenuBar();
		}

		if (showInspector)
		{
			ImGui::Begin("Inspector", &showInspector);
			ShowEditorInspector();
			ImGui::End();
		}

		if (showHierarchy)
		{
			ImGui::Begin("Hierarchy", &showHierarchy);
			ShowEditorHierarchy();
			ImGui::End();
		}

		if (showScene)
		{
			ImGui::Begin("Scene", &showScene);
			ShowEditorSceneImage();
			ImGui::End();
		}

		if (showGame)
		{
			ImGui::Begin("Game", &showGame);
			ImGui::Text("todo");
			ImGui::End();
		}

		if (showProject)
		{
			ImGui::Begin("Project", &showProject);
			ShowEditorProject();
			ImGui::End();
		}

		if (showConsole)
		{
			ImGui::Begin("Console", &showConsole);
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}
	}

}
