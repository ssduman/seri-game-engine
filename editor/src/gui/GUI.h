#pragma once

#include <seri/core/Seri.h>

#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <misc/freetype/imgui_freetype.h>

#include "event/UserEvent.h"

#include <memory>

#if defined (SERI_USE_WINDOW_GLFW)
#include <GLFW/glfw3.h>
#include <backends/imgui_impl_glfw.h>
#elif defined (SERI_USE_WINDOW_SDL3)
#include <SDL3/SDL.h>
#include <backends/imgui_impl_sdl3.h>
#endif

namespace seri::editor
{
	class GUI : public seri::Object
	{
	public:
		GUI()
		{
			LOGGER(info, "[gui] init succeeded");
		}

		~GUI() override
		{
			ImGui_ImplOpenGL3_Shutdown();

#if defined (SERI_USE_WINDOW_GLFW)
			ImGui_ImplGlfw_Shutdown();
#elif defined (SERI_USE_WINDOW_SDL3)
			ImGui_ImplSDL3_Shutdown();
#endif

			ImGui::DestroyContext();

			LOGGER(info, "[gui] delete succeeded");
		}

		void Init() override
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

		void Update() override
		{
			ImGui_ImplOpenGL3_NewFrame();
#if defined (SERI_USE_WINDOW_GLFW)
			ImGui_ImplGlfw_NewFrame();
#elif defined (SERI_USE_WINDOW_SDL3)
			ImGui_ImplSDL3_NewFrame();
#endif
			ImGui::NewFrame();

			DrawEditorLayout();
		}

		void Render() override
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

		void ProcessEvent(const void* event)
		{
#if defined (SERI_USE_WINDOW_SDL3)
			ImGui_ImplSDL3_ProcessEvent(static_cast<const SDL_Event*>(event));
#endif
		}

	private:
		void SetIO()
		{
			auto& io = ImGui::GetIO();
			io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
			io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
			io.ConfigWindowsMoveFromTitleBarOnly = true;
		}

		void SetStyle()
		{
			ImGui::StyleColorsDark();
			auto& style = ImGui::GetStyle();

			style.GrabRounding = 6.0f;
			style.ChildRounding = 6.0f;
			style.FrameRounding = 6.0f;
			style.PopupRounding = 6.0f;
			style.WindowRounding = 6.0f;
			style.ScrollbarRounding = 6.0f;

			style.FrameBorderSize = 1.0f;
			style.WindowTitleAlign = ImVec2(0.5f, 0.50f);
			style.WindowMenuButtonPosition = ImGuiDir_Right;

			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
			style.Colors[ImGuiCol_TitleBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
			style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
			style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
		}
		
		void ShowEditorFrambufferImage()
		{
			auto tex = (ImTextureID)(intptr_t)seri::RenderingManager::GetEditorRT()->GetFirstColorTextureHandle();

			auto w = seri::RenderingManager::GetEditorRT()->GetWidth();
			auto h = seri::RenderingManager::GetEditorRT()->GetHeight();

			ImGui::Image(
				tex,
				ImVec2(w, h),
				ImVec2(0, 1),
				ImVec2(1, 0)
			);
		}

		void DrawEditorLayout()
		{
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

			if (showHierarchy)
			{
				ImGui::Begin("Hierarchy", &showHierarchy);
				ImGui::Text("todo");
				ImGui::End();
			}

			if (showScene)
			{
				ImGui::Begin("Scene", &showScene);
				ShowEditorFrambufferImage();
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
				ImGui::Text("todo");
				ImGui::End();
			}

			if (showInspector)
			{
				ImGui::Begin("Inspector", &showInspector);
				ImGui::Text("todo");
				ImGui::End();
			}

			if (showConsole)
			{
				ImGui::Begin("Console", &showConsole);
				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
				ImGui::End();
			}
		}

	};
}
