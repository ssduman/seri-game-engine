#pragma once
#pragma warning(disable: 4100)

#include "seri/window/IWindowManager.h"
#include "seri/renderer/AuxiliaryStructs.h"
#include "seri/input/InputManager.h"

#include <string>
#include <utility>
#include <stdexcept>

#define SDL_MAIN_HANDLED
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

namespace seri
{
	class WindowsSDLWindowManager : public IWindowManager
	{
	public:
		WindowsSDLWindowManager() = default;

		~WindowsSDLWindowManager() override
		{
			SDL_GL_DestroyContext(_context);
			SDL_DestroyWindow(_window);
			SDL_Quit();

			LOGGER(info, "[window] windows sdl window manager destroyed and terminated successfully");
		}

		void Init() override
		{
			if (_initialized)
			{
				throw std::runtime_error("[window] window manager is already initialized");
			}

			InitSDL();

			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
			SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

			CreateSDLWindow();
			SetContext();

			InitGlad();
			LogGLInfo();
			SetGLOptions();

			SetWindowUserPointer(static_cast<void*>(this));

			_initialized = true;

			LOGGER(info, "[window] sdl window manager created successfully");
		}

		double GetTime() override
		{
			return (double)SDL_GetTicksNS() / 1e9;
		}

		void* GetWindowHandle() override
		{
			return _window;
		}

		void* GetContext() override
		{
			return _context;
		}

		void SetCursorMode(CursorMode cursorMode) override
		{
			switch (cursorMode)
			{
				case seri::normal:
					SDL_ShowCursor();
					SDL_SetWindowRelativeMouseMode(_window, false);
					break;
				case seri::hidden:
					SDL_HideCursor();
					SDL_SetWindowRelativeMouseMode(_window, false);
					break;
				case seri::disabled:
					SDL_HideCursor();
					SDL_SetWindowRelativeMouseMode(_window, true);
					break;
				default:
					LOGGER(info, "[window] unexpected cursor mode");
					break;
			}
		}

		std::pair<double, double> GetCursorPosition() override
		{
			float mouseXPosition, mouseYPosition;
			SDL_GetMouseState(&mouseXPosition, &mouseYPosition);
			return { mouseXPosition, mouseYPosition };
		}

		void SetCursorPosition(double xpos, double ypos) override
		{
			SDL_WarpMouseInWindow(_window, xpos, ypos);
		}

		void SetVSyncCount(int count) override
		{
			bool ret = SDL_GL_SetSwapInterval(count);
			if (!ret)
			{
				LOGGER(error, "[window] sdl set vsync error: " + std::string(SDL_GetError()));
			}
		}

		void PollEvents() override
		{
			SDL_Event event;
			while (SDL_PollEvent(&event))
			{
				for (const auto& delegate : processEventDelegateList) {
					if (delegate) {
						delegate((const void*)&event);
					}
				}

				switch ((SDL_EventType)event.type)
				{
					case SDL_EVENT_QUIT:
						{
							_shouldClose = true;
							LOGGER(info, "[window] sdl event: quit");
						}
						break;
					case SDL_EVENT_WINDOW_SHOWN:
						{
							LOGGER(info, "[window] sdl event: window shown");
						}
						break;
					case SDL_EVENT_WINDOW_HIDDEN:
						{
							LOGGER(info, "[window] sdl event: window hidden");
						}
						break;
					case SDL_EVENT_WINDOW_MOVED:
						{
							LOGGER(info, "[window] sdl event: window moved");
						}
						break;
					case SDL_EVENT_WINDOW_RESIZED:
						{
							LOGGER(info, "[window] sdl event: window resized");
						}
						break;
					case SDL_EVENT_WINDOW_MINIMIZED:
						{
							LOGGER(info, "[window] sdl event: window minimized");
						}
						break;
					case SDL_EVENT_WINDOW_MAXIMIZED:
						{
							LOGGER(info, "[window] sdl event: window maximized");
						}
						break;
					case SDL_EVENT_WINDOW_MOUSE_ENTER:
						{
							LOGGER(info, "[window] sdl event: mouse enter");
						}
						break;
					case SDL_EVENT_WINDOW_MOUSE_LEAVE:
						{
							LOGGER(info, "[window] sdl event: mouse leave");
						}
						break;
					case SDL_EVENT_KEY_DOWN:
						{
							LOGGER(info, "[window] sdl event: key down");
						}
						break;
					case SDL_EVENT_KEY_UP:
						{
							LOGGER(info, "[window] sdl event: key up");
						}
						break;
					case SDL_EVENT_MOUSE_MOTION:
						{
							//LOGGER(info, "[window] sdl event: mouse move");
						}
						break;
					case SDL_EVENT_MOUSE_BUTTON_DOWN:
						{
							LOGGER(info, "[window] sdl event: mouse button down");
						}
						break;
					case SDL_EVENT_MOUSE_BUTTON_UP:
						{
							LOGGER(info, "[window] sdl event: mouse button up");
						}
						break;
					case SDL_EVENT_MOUSE_WHEEL:
						{
							LOGGER(info, "[window] sdl event: mouse wheel");
						}
						break;
					default:
						break;
				}
			}
		}

		void SwapBuffers() override
		{
			SDL_GL_SwapWindow(_window);
		}

		bool GetWindowShouldClose() override
		{
			return _shouldClose;
		}

		void SetWindowShouldCloseToTrue() override
		{
			_shouldClose = true;
		}

		std::pair<int, int> GetWindowPosition() override
		{
			int xpos, ypos;
			SDL_GetWindowPosition(_window, &xpos, &ypos);
			return { xpos, ypos };
		}

		void SetWindowPosition(int xpos, int ypos) override
		{
			SDL_SetWindowPosition(_window, xpos, ypos);
		}

		const char* GetClipboard() override
		{
			return SDL_GetClipboardText();
		}

		void SetClipboard(const char* str) override
		{
			SDL_SetClipboardText(str);
		}

		void SetWindowUserPointer(void* pointer)
		{
			SDL_SetPointerProperty(SDL_GetWindowProperties(_window), "user.data", pointer);
		}

		void* GetWindowUserPointer()
		{
			return SDL_GetPointerProperty(SDL_GetWindowProperties(_window), "user.data", nullptr);;
		}

	protected:
		void InitGlad() override
		{
			int version = gladLoadGL(SDL_GL_GetProcAddress);
			if (version == 0) {
				throw std::runtime_error("[window] glad load error");
			}

			LOGGER(info, "[window] loaded opengl " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version));
		}

	private:
		void InitSDL()
		{
			if (!SDL_Init(SDL_INIT_VIDEO)) {
				throw std::runtime_error("[window] sdl init error: " + std::string(SDL_GetError()));
			}

			LOGGER(info, "[window] sdl version '" << SDL_MAJOR_VERSION << "." << SDL_MINOR_VERSION << "." << SDL_MICRO_VERSION << "' init succeeded");
		}

		void CreateSDLWindow()
		{
			SDL_WindowFlags flags =
				SDL_WINDOW_OPENGL |
				SDL_WINDOW_RESIZABLE
				;

			if (_windowProperties.isFullscreen)
			{
				flags |= SDL_WINDOW_FULLSCREEN;
			}

			_window = SDL_CreateWindow(
				_windowProperties.windowTitle,
				_windowProperties.windowWidth, _windowProperties.windowHeight,
				flags
			);

			if (!_window)
			{
				LOGGER(error, "[window] sdl window creating error: " + std::string(SDL_GetError()));
				SDL_Quit();
			}

			LOGGER(info, "[window] sdl window created");
		}

		void SetContext()
		{
			_context = SDL_GL_CreateContext(_window);
			if (!_context)
			{
				LOGGER(error, "[window] sdl gl create context error: " + std::string(SDL_GetError()));
				SDL_DestroyWindow(_window);
				SDL_Quit();
			}
		}

		SDL_Window* _window{ nullptr };
		SDL_GLContext _context;

		bool _shouldClose{ false };

	};
}
