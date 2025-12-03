#pragma once

#include <seri/core/Seri.h>

#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <misc/freetype/imgui_freetype.h>

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

			ShowDemoWindow();

			ShowMainMenuBar();

			ShowImage();

			ShowSceneWindow();

			ShowEntityWindow();
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
		void HelpMarker(const char* desc)
		{
			ImGui::TextDisabled("(?)");
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
				ImGui::TextUnformatted(desc);
				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();
			}
		}

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

		void ShowDemoWindow()
		{
			static bool show_demo_window = false;
			if (show_demo_window)
			{
				ImGui::ShowDemoWindow(&show_demo_window);
			}
		}

		void ShowMainMenuBar()
		{
			static bool show_menu_window = true;
			if (show_menu_window)
			{
				if (!ImGui::Begin("Seri", &show_menu_window))
				{
					ImGui::End();
					return;
				}

				if (ImGui::BeginMainMenuBar())
				{
					if (ImGui::BeginMenu("File"))
					{
						ShowMenuFile();
						ImGui::EndMenu();
					}
					if (ImGui::BeginMenu("Edit"))
					{
						if (ImGui::MenuItem("Undo", "CTRL+Z"))
						{
						}
						if (ImGui::MenuItem("Redo", "CTRL+Y", false, false))
						{
						}
						ImGui::Separator();
						if (ImGui::MenuItem("Cut", "CTRL+X"))
						{
						}
						if (ImGui::MenuItem("Copy", "CTRL+C"))
						{
						}
						if (ImGui::MenuItem("Paste", "CTRL+V"))
						{
						}
						ImGui::EndMenu();
					}

					ButtonPlayCenter();

					TextRight("Seri Game Engine");

					ImGui::EndMainMenuBar();
				}

				ImGui::End();
			}
		}

		void ShowImage()
		{
			ImGui::Begin("Editor");

			auto tex = (ImTextureID)(intptr_t)seri::RenderingManager::GetEditorRT()->GetFirstColorTextureHandle();

			auto w = seri::RenderingManager::GetEditorRT()->GetWidth();
			auto h = seri::RenderingManager::GetEditorRT()->GetHeight();

			ImGui::Image(
				tex,
				ImVec2(w, h),
				ImVec2(0, 1),
				ImVec2(1, 0)
			);

			ImGui::End();
		}

		void ShowSceneWindow()
		{
		}

		void ShowMenuFile()
		{
			if (ImGui::MenuItem("New"))
			{
			}
			if (ImGui::MenuItem("Open", "Ctrl+O"))
			{
			}
			if (ImGui::MenuItem("Save", "Ctrl+S"))
			{
			}
			if (ImGui::MenuItem("Save As.."))
			{
			}
			if (ImGui::MenuItem("Quit", "Alt+F4"))
			{
				seri::WindowManager::SetWindowShouldCloseToTrue();
			}
		}

		void ShowEntityWindow()
		{
			static bool show_entity_window = true;
			if (show_entity_window)
			{
				auto windowFlags = 0;
				windowFlags |= ImGuiWindowFlags_MenuBar;
				if (!ImGui::Begin("Components", &show_entity_window, windowFlags))
				{
					ImGui::End();
					return;
				}

				ShowEntityWindowMenuBar();

				ShowEntityTransformationOptions();

				ShowGUIOptions();

				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

				ImGui::End();
			}
		}

		void ShowEntityWindowMenuBar()
		{
			ImGui::PushItemWidth(ImGui::GetFontSize() * -12);

			static bool show_app_main_menu_bar = false;
			static bool show_app_example_menu_bar = false;
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("Menu"))
				{
					ImGui::MenuItem("Option 1", nullptr, &show_app_main_menu_bar);
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Settings"))
				{
					ImGui::MenuItem("Setting 1", nullptr, &show_app_example_menu_bar);
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}
		}

		void ShowEntityTransformationOptions()
		{
			if (_currentEntity)
			{
				_currentEntity->GetShader()->Bind();

				ImGui::SliderFloat3("position", &_currentEntity->GetTransform()._position[0], -1.0f, 1.0f, "%.4f");
				ImGui::SliderFloat3("rotation", &_currentEntity->GetTransform()._rotation[0], -180.0f, 180.0f, "%.4f");
				ImGui::SliderFloat3("scale", &_currentEntity->GetTransform()._scale[0], 0.0f, 100.0f, "%.4f");
				seri::ShaderLibrary::SetModel(_currentEntity->GetShader(), _currentEntity->GetTransform().Apply());

				ImGui::Separator();

				_currentEntity->GetShader()->Unbind();
			}
			else if (_currentObject)
			{
				ImGui::Separator();

				glm::vec3 pos = _currentObject->GetPosition();
				ImGui::InputFloat3("position", &pos[0]);
				_currentObject->SetPosition(pos);

				ImGui::Separator();
			}
		}

		void ShowGUIOptions()
		{
			if (ImGui::Button("Enable cursor"))
			{
				seri::WindowManager::SetCursorMode(seri::CursorMode::normal);
			}
			if (ImGui::Button("Disable cursor"))
			{
				seri::WindowManager::SetCursorMode(seri::CursorMode::disabled);
			}

			ImGui::Separator();
		}

		void TextCenter(const std::string& text)
		{
			auto windowWidth = ImGui::GetWindowSize().x;
			auto textWidth = ImGui::CalcTextSize(text.c_str()).x;

			ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
			ImGui::Button(text.c_str());
		}

		void TextRight(const std::string& text)
		{
			auto windowWidth = ImGui::GetWindowSize().x;
			auto textWidth = ImGui::CalcTextSize(text.c_str()).x;

			ImGui::SetCursorPosX(windowWidth - textWidth - 5.0f);
			ImGui::Text(text.c_str());
		}

		void ButtonPlayCenter()
		{
			static std::string text = "Play";
			auto windowWidth = ImGui::GetWindowSize().x;
			auto textWidth = ImGui::CalcTextSize(text.c_str()).x;

			ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);

			ImGui::PushStyleColor(ImGuiCol_Button, static_cast<ImVec4>(ImColor::HSV(0.3f, 0.6f, 0.6f)));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, static_cast<ImVec4>(ImColor::HSV(0.3f, 0.7f, 0.7f)));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, static_cast<ImVec4>(ImColor::HSV(0.3f, 0.8f, 0.8f)));
			if (ImGui::Button(text.c_str()))
			{
				//if (text == "Play")
				//{
				//	text = "Stop";
				//	seri::WindowManager::FireEvent(UserGameStateEventData{ GameState::game });
				//	seri::WindowManager::SetCursorMode(seri::CursorMode::disabled);
				//	//_io->MouseDrawCursor = true;
				//}
				//else
				//{
				//	text = "Play";
				//	seri::WindowManager::FireEvent(UserGameStateEventData{ GameState::idle });
				//	seri::WindowManager::SetCursorMode(seri::CursorMode::disabled);
				//	//_io->MouseDrawCursor = false;
				//}
			}
			ImGui::PopStyleColor(3);
		}

		void RegisterEntity(std::shared_ptr<Object> entity)
		{
			_currentEntity = std::dynamic_pointer_cast<seri::Entity>(entity);
			_currentObject = std::dynamic_pointer_cast<seri::Object>(entity);
		}

		std::shared_ptr<seri::Entity> _currentEntity;
		std::shared_ptr<seri::Object> _currentObject;

	};
}
