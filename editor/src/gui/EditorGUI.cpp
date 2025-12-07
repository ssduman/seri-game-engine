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

		ImVec2 finalSize;
		if (panelAspect > fbAspect)
		{
			finalSize.y = panelSize.y;
			finalSize.x = panelSize.y * fbAspect;
		}
		else
		{
			finalSize.x = panelSize.x;
			finalSize.y = panelSize.x / fbAspect;
		}

		ImVec2 cursorPos = ImGui::GetCursorScreenPos();

		float offsetX = (panelSize.x - finalSize.x) * 0.5f;
		float offsetY = (panelSize.y - finalSize.y) * 0.5f;

		ImVec2 drawPos = ImVec2(cursorPos.x + offsetX, cursorPos.y + offsetY);

		auto tex = (ImTextureID)(intptr_t)seri::RenderingManager::GetEditorRT()->GetFirstColorTextureHandle();
		ImGui::GetWindowDrawList()->AddImage(
			tex,
			drawPos,
			ImVec2(drawPos.x + finalSize.x, drawPos.y + finalSize.y),
			ImVec2(0, 1),
			ImVec2(1, 0)
		);
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

			auto* idComponent = seri::scene::SceneManager::GetRegistry().try_get<seri::IDComponent>(entity);
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

		if (auto* idComponent = registry.try_get<seri::IDComponent>(entity))
		{
			ImGui::Text(fmt::format("id: {}", idComponent->id).c_str());
			ImGui::Text(fmt::format("parent id: {}", idComponent->parentId).c_str());
			ImGui::Text(fmt::format("name: {}", idComponent->name.c_str()).c_str());

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strncpy_s(buffer, idComponent->name.c_str(), sizeof(buffer) - 1);
			if (ImGui::InputText("name", buffer, sizeof(buffer)))
			{
				idComponent->name = std::string{ buffer };
				scene->SetAsDirty();
			}

			ImGui::Separator();
		}

		if (auto* transformComponent = registry.try_get<seri::TransformComponent>(entity))
		{
			ImGui::InputFloat3("position", &transformComponent->position[0]);
			ImGui::InputFloat3("rotation", &transformComponent->rotation[0]);
			ImGui::InputFloat3("scale", &transformComponent->scale[0]);

			ImGui::Separator();
		}
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
			ImGui::BeginGroup();

			ImGui::Button(child.name.c_str(), ImVec2(itemSize, itemSize));

			//if (ImGui::Selectable(child.name.c_str(), _selectedFolder->path == child.path)) {}

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
