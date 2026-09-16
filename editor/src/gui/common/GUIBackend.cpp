#include "Editorpch.h"

#include "gui/common/GUIBackend.h"

#include <imgui_impl_opengl3.h>

#if defined (SERI_USE_WINDOW_GLFW)
#include <GLFW/glfw3.h>
#include <imgui_impl_glfw.h>
#elif defined (SERI_USE_WINDOW_SDL3)
#include <SDL3/SDL.h>
#include <imgui_impl_sdl3.h>
#endif

namespace seri::editor
{
	void GUIBackend::Init()
	{
#if defined (SERI_USE_WINDOW_GLFW)
		ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(seri::WindowManager::GetWindowHandle()), true);
#elif defined (SERI_USE_WINDOW_SDL3)
		ImGui_ImplSDL3_InitForOpenGL(static_cast<SDL_Window*>(seri::WindowManager::GetWindowHandle()), seri::WindowManager::GetContext());
#endif

		ImGui_ImplOpenGL3_Init("#version 460");
	}

	void GUIBackend::Shutdown()
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
	}

	void GUIBackend::NewFrame()
	{
		ImGui_ImplOpenGL3_NewFrame();

#if defined (SERI_USE_WINDOW_GLFW)
		ImGui_ImplGlfw_NewFrame();
#elif defined (SERI_USE_WINDOW_SDL3)
		ImGui_ImplSDL3_NewFrame();
#endif

		ImGui::NewFrame();
	}

	void GUIBackend::Render()
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

	void GUIBackend::ProcessEvent(const void* event)
	{
		if (ImGui::GetCurrentContext() == nullptr)
		{
			return;
		}

#if defined (SERI_USE_WINDOW_SDL3)
		ImGui_ImplSDL3_ProcessEvent(static_cast<const SDL_Event*>(event));
#endif
	}
}
