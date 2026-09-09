#include "gui/EditorGUI.h"
#include "gui/EditorStyle.h"

#include <imgui_impl_opengl3.h>
#include <imgui_freetype.h>

#if defined (SERI_USE_WINDOW_GLFW)
#include <GLFW/glfw3.h>
#include <imgui_impl_glfw.h>
#elif defined (SERI_USE_WINDOW_SDL3)
#include <SDL3/SDL.h>
#include <imgui_impl_sdl3.h>
#endif

namespace seri::editor
{
	EditorGUI::EditorGUI()
	{
		LIB_LOGGER(info, gui) << "created, imgui version: " << IMGUI_VERSION;
	}

	EditorGUI::~EditorGUI()
	{
		if (ImGui::GetCurrentContext() == nullptr)
		{
			LIB_LOGGER(warning, gui) << "unexpected context to destroy";
			return;
		}

		ImGui_ImplOpenGL3_Shutdown();

#if defined (SERI_USE_WINDOW_GLFW)
		ImGui_ImplGlfw_Shutdown();
#elif defined (SERI_USE_WINDOW_SDL3)
		ImGui_ImplSDL3_Shutdown();
#endif

		ImGui::DestroyContext();

		LIB_LOGGER(info, gui) << "destroyed";
	}

	void EditorGUI::Init()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		SetIO();
		SetFonts();
		SetStyle();

		_titleBar.Init();

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

		_consolePanel.Update();
		_gamePanel.Update(_context);

		DrawLayout();

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

		if (io.WantTextInput)
		{
			return;
		}

		if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_S, false))
		{
			Save();
		}

		if (ImGui::IsKeyPressed(ImGuiKey_Escape, false) &&
			!ImGui::IsPopupOpen(nullptr, ImGuiPopupFlags_AnyPopupId | ImGuiPopupFlags_AnyPopupLevel))
		{
			if (seri::scene::SceneManager::GetState() != seri::scene::SceneState::edit)
			{
				seri::scene::SceneManager::SetState(seri::scene::SceneState::edit);
			}
		}
	}

	void EditorGUI::BuildDefaultDockLayout(ImGuiID dockspaceId)
	{
		ImGui::DockBuilderRemoveNode(dockspaceId);
		ImGui::DockBuilderAddNode(dockspaceId, ImGuiDockNodeFlags_DockSpace);
		ImGui::DockBuilderSetNodeSize(dockspaceId, ImGui::GetMainViewport()->WorkSize);

		ImGuiID dockCenter = dockspaceId;
		ImGuiID dockLeft = ImGui::DockBuilderSplitNode(dockCenter, ImGuiDir_Left, 0.15f, nullptr, &dockCenter);
		ImGuiID dockBottom = ImGui::DockBuilderSplitNode(dockCenter, ImGuiDir_Down, 0.35f, nullptr, &dockCenter);
		ImGuiID dockRight = ImGui::DockBuilderSplitNode(dockCenter, ImGuiDir_Right, 0.30f, nullptr, &dockCenter);
		ImGuiID dockBottomRight = ImGui::DockBuilderSplitNode(dockBottom, ImGuiDir_Right, 0.30f, nullptr, &dockBottom);

		ImGui::DockBuilderDockWindow("Hierarchy", dockLeft);
		ImGui::DockBuilderDockWindow("Game", dockCenter);
		ImGui::DockBuilderDockWindow("Scene", dockCenter);
		ImGui::DockBuilderDockWindow("Inspector", dockRight);
		ImGui::DockBuilderDockWindow("Project", dockBottom);
		ImGui::DockBuilderDockWindow("Console", dockBottomRight);

		ImGui::DockBuilderFinish(dockspaceId);

		LIB_LOGGER(info, gui) << "default dock layout built";
	}

	void EditorGUI::DrawLayout()
	{
		ImGuiWindowFlags window_flags =
			ImGuiWindowFlags_NoDocking |
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoNavFocus |
			ImGuiWindowFlags_NoBackground
			;

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		ImGui::Begin("Editor DockSpace", nullptr, window_flags);

		ImGui::PopStyleVar(3);

		ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");

		if (_context.resetLayout || ImGui::DockBuilderGetNode(dockspace_id) == nullptr)
		{
			BuildDefaultDockLayout(dockspace_id);
			_context.resetLayout = false;
		}

		const float tabPadding = 4.0f;
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(tabPadding, ImGui::GetStyle().FramePadding.y));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(4.0f, ImGui::GetStyle().ItemInnerSpacing.y));
		// negative border cancels the FramePadding.x offset ImGui adds before the first dock tab, so tabs start at the node edge
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, -tabPadding);
		ImGui::DockSpace(dockspace_id, ImVec2(0, 0), ImGuiDockNodeFlags_PassthruCentralNode);
		ImGui::PopStyleVar(3);

		ImGui::End();

		_titleBar.Draw(_context);

		if (_context.showInspector)
		{
			if (ImGui::Begin("Inspector", &_context.showInspector))
			{
				_inspectorPanel.Draw(_context);
			}
			ImGui::End();
		}

		if (_context.showHierarchy)
		{
			if (ImGui::Begin("Hierarchy", &_context.showHierarchy))
			{
				_hierarchyPanel.Draw(_context);
			}
			ImGui::End();
		}

		if (_context.showScene)
		{
			if (ImGui::Begin("Scene", &_context.showScene))
			{
				_scenePanel.Draw(_context);
			}
			ImGui::End();
		}

		if (_context.showGame)
		{
			if (_gamePanel.IsMaximized())
			{
				ImGuiWindowFlags flags =
					ImGuiWindowFlags_NoDocking |
					ImGuiWindowFlags_NoResize |
					ImGuiWindowFlags_NoMove |
					ImGuiWindowFlags_NoCollapse |
					ImGuiWindowFlags_NoSavedSettings
					;

				ImGui::SetNextWindowPos(viewport->WorkPos);
				ImGui::SetNextWindowSize(viewport->WorkSize);
				ImGui::SetNextWindowViewport(viewport->ID);

				if (ImGui::Begin("Game##maximized", &_context.showGame, flags))
				{
					_gamePanel.Draw(_context);
				}
				ImGui::End();
			}
			else
			{
				if (ImGui::Begin("Game", &_context.showGame))
				{
					_gamePanel.Draw(_context);
				}
				ImGui::End();
			}
		}

		if (_context.showProject)
		{
			if (ImGui::Begin("Project", &_context.showProject))
			{
				_projectPanel.Draw(_context);
			}
			ImGui::End();
		}

		if (_context.showConsole)
		{
			if (ImGui::Begin("Console", &_context.showConsole))
			{
				_consolePanel.Draw();
			}
			ImGui::End();
		}
	}
}
