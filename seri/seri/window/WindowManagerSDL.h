#pragma once
#pragma warning(disable: 4100)

#include "seri/input/InputManager.h"
#include "seri/window/WindowManagerBase.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

#include <string>
#include <utility>
#include <stdexcept>

namespace seri
{
	class WindowManagerSDL : public WindowManagerBase
	{
	public:
		WindowManagerSDL() = default;

		~WindowManagerSDL() override
		{
			SDL_GL_DestroyContext(_context);
			SDL_DestroyWindow(_window);
			SDL_Quit();

			LIB_LOGGER(info, window) << "sdl window manager destroyed and terminated successfully";
		}

		void Init(WindowProperties windowProperties) override
		{
			if (_initialized)
			{
				throw std::runtime_error("[window] sdl window manager is already initialized");
			}

			_windowProperties = windowProperties;

			InitSDL();
			CreateWindowSDL();
			SetWindowUserPointer(static_cast<void*>(this));

			_initialized = true;

			LIB_LOGGER(info, window) << "sdl window manager created successfully";
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
					LIB_LOGGER(info, window) << "unexpected cursor mode";
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
				LIB_LOGGER(error, window) << "sdl set vsync error: " + std::string(SDL_GetError());
			}
		}

		void PollEvents() override
		{
			SDL_Event event;
			while (SDL_PollEvent(&event))
			{
				for (const auto& delegate : processEventDelegateList)
				{
					if (delegate)
					{
						delegate((const void*)&event);
					}
				}

				switch ((SDL_EventType)event.type)
				{
					case SDL_EVENT_QUIT:
						{
							_shouldClose = true;

							LIB_LOGGER(info, window) << "sdl event: quit";
						}
						break;
					case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
						{
							FireEvent(event::WindowCloseEventData{});

							LIB_LOGGER(info, window) << "sdl event: window closing";
						}
						break;
					case SDL_EVENT_WINDOW_SHOWN:
						{
							LIB_LOGGER(info, window) << "sdl event: window shown";
						}
						break;
					case SDL_EVENT_WINDOW_HIDDEN:
						{
							LIB_LOGGER(info, window) << "sdl event: window hidden";
						}
						break;
					case SDL_EVENT_WINDOW_MOVED:
						{
							int x = event.window.data1;
							int y = event.window.data2;

							//LIB_LOGGER(info, window) << "sdl event: window moved: " << x << ", " << y;
						}
						break;
					case SDL_EVENT_WINDOW_MINIMIZED:
						{
							LIB_LOGGER(info, window) << "sdl event: window minimized";
						}
						break;
					case SDL_EVENT_WINDOW_MAXIMIZED:
						{
							LIB_LOGGER(info, window) << "sdl event: window maximized";
						}
						break;
					case SDL_EVENT_WINDOW_FOCUS_GAINED:
						{
							LIB_LOGGER(info, window) << "sdl event: window focused";
						}
						break;
					case SDL_EVENT_WINDOW_FOCUS_LOST:
						{
							LIB_LOGGER(info, window) << "sdl event: window not focused";
						}
						break;
					case SDL_EVENT_WINDOW_RESIZED:
						{
							int w = event.window.data1;
							int h = event.window.data2;

							SetViewport(0, 0, w, h);

							FireEvent(event::WindowResizeEventData{ w, h });

							//LIB_LOGGER(info, window) << "sdl event: window resized: " << w << ", " << h;
						}
						break;
					case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
						{
							int w = event.window.data1;
							int h = event.window.data2;

							SetViewport(0, 0, w, h);

							FireEvent(event::WindowResizeEventData{ w, h });

							//LIB_LOGGER(info, window) << "sdl event: window pixel size changed: " << w << ", " << h;
						}
						break;
					case SDL_EVENT_WINDOW_MOUSE_ENTER:
						{
							FireEvent(event::MouseEnterEventData{ true });

							//LIB_LOGGER(info, window) << "sdl event: mouse enter";
						}
						break;
					case SDL_EVENT_WINDOW_MOUSE_LEAVE:
						{
							FireEvent(event::MouseEnterEventData{ false });

							//LIB_LOGGER(info, window) << "sdl event: mouse leave";
						}
						break;
					case SDL_EVENT_MOUSE_MOTION:
						{
							double xpos = event.motion.x;
							double ypos = event.motion.y;

							FireEvent(event::MousePositionEventData{ xpos, ypos });

							InputManager::RegisterCursorPosition(xpos, ypos);

							//LIB_LOGGER(info, window) << "sdl event: mouse move";
						}
						break;
					case SDL_EVENT_MOUSE_BUTTON_DOWN:
						{
							MouseButtonCode buttonEnum = MouseButtonCode::noop;
							InputAction actionEnum = InputAction::press;
							InputModifier modsEnum = InputModifier::noop;

							switch (event.button.button)
							{
								case SDL_BUTTON_LEFT:
									buttonEnum = MouseButtonCode::button_left;
									break;
								case SDL_BUTTON_MIDDLE:
									buttonEnum = MouseButtonCode::button_middle;
									break;
								case SDL_BUTTON_RIGHT:
									buttonEnum = MouseButtonCode::button_right;
									break;
								case SDL_BUTTON_X1:
									buttonEnum = MouseButtonCode::button_4;
									break;
								case SDL_BUTTON_X2:
									buttonEnum = MouseButtonCode::button_5;
									break;
								default:
									break;
							}

							if (buttonEnum == MouseButtonCode::noop)
							{
								LIB_LOGGER(info, window) << "sdl event: mouse button down: unexpected button type";
								break;
							}

							FireEvent(event::MouseButtonEventData{ buttonEnum, actionEnum, modsEnum });

							InputManager::RegisterMouse(buttonEnum, actionEnum);

							//LIB_LOGGER(info, window) << "sdl event: mouse button down";
						}
						break;
					case SDL_EVENT_MOUSE_BUTTON_UP:
						{
							MouseButtonCode buttonEnum = MouseButtonCode::noop;
							InputAction actionEnum = InputAction::release;
							InputModifier modsEnum = InputModifier::noop;

							switch (event.button.button)
							{
								case SDL_BUTTON_LEFT:
									buttonEnum = MouseButtonCode::button_left;
									break;
								case SDL_BUTTON_MIDDLE:
									buttonEnum = MouseButtonCode::button_middle;
									break;
								case SDL_BUTTON_RIGHT:
									buttonEnum = MouseButtonCode::button_right;
									break;
								case SDL_BUTTON_X1:
									buttonEnum = MouseButtonCode::button_4;
									break;
								case SDL_BUTTON_X2:
									buttonEnum = MouseButtonCode::button_5;
									break;
								default:
									break;
							}

							if (buttonEnum == MouseButtonCode::noop)
							{
								LIB_LOGGER(info, window) << "sdl event: mouse button up: unexpected button type";
								break;
							}

							FireEvent(event::MouseButtonEventData{ buttonEnum, actionEnum, modsEnum });

							InputManager::RegisterMouse(buttonEnum, actionEnum);

							//LIB_LOGGER(info, window) << "sdl event: mouse button up";
						}
						break;
					case SDL_EVENT_MOUSE_WHEEL:
						{
							double xoffset = event.wheel.x;
							double yoffset = event.wheel.y;

							FireEvent(event::MouseScrollEventData{ xoffset, yoffset });

							InputManager::RegisterScrollDelta(xoffset, yoffset);

							//LIB_LOGGER(info, window) << "sdl event: mouse wheel";
						}
						break;
					case SDL_EVENT_KEY_DOWN:
						{
							int scancode = event.key.scancode;
							int key = event.key.key;
							int mods = event.key.mod;
							bool down = event.key.down;
							bool repeat = event.key.repeat;

							KeyCode keyEnum = GetKeyCodeEnum(key);
							InputAction actionEnum = repeat ? InputAction::repeat : InputAction::press;
							std::vector<InputModifier> modsVector;
							FillModsVector(mods, modsVector);

							if (keyEnum == KeyCode::unknown)
							{
								LIB_LOGGER(info, window) << "sdl event: key down: unexpected key type";
								break;
							}

							FireEvent(event::KeyEventData{ keyEnum, scancode, actionEnum, std::move(modsVector) });

							InputManager::RegisterKey(keyEnum, actionEnum);

							if (keyEnum == KeyCode::escape && actionEnum == InputAction::press)
							{
								SetWindowShouldCloseToTrue();
							}

							//LIB_LOGGER(info, window) << "sdl event: key down: " << seri::ToString(keyEnum) << ", repeat: " << seri::ToString(actionEnum);
						}
						break;
					case SDL_EVENT_KEY_UP:
						{
							int scancode = event.key.scancode;
							int key = event.key.key;
							int mods = event.key.mod;
							bool down = event.key.down;
							bool repeat = event.key.repeat;

							KeyCode keyEnum = GetKeyCodeEnum(key);
							InputAction actionEnum = InputAction::release;
							std::vector<InputModifier> modsVector;
							FillModsVector(mods, modsVector);

							if (keyEnum == KeyCode::unknown)
							{
								LIB_LOGGER(info, window) << "sdl event: key up: unexpected key type";
								break;
							}

							FireEvent(event::KeyEventData{ keyEnum, scancode, actionEnum, std::move(modsVector) });

							InputManager::RegisterKey(keyEnum, actionEnum);

							if (keyEnum == KeyCode::escape)
							{
								SetWindowShouldCloseToTrue();
							}

							//LIB_LOGGER(info, window) << "sdl event: key up: " << seri::ToString(keyEnum);
						}
						break;
					case SDL_EVENT_TEXT_INPUT:
						{
							LIB_LOGGER(info, window) << "sdl event: text input";
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

		void SetWindowUserPointer(void* pointer) override
		{
			SDL_SetPointerProperty(SDL_GetWindowProperties(_window), "user.pointer", pointer);
		}

		void* GetWindowUserPointer() override
		{
			return SDL_GetPointerProperty(SDL_GetWindowProperties(_window), "user.pointer", nullptr);;
		}

		void* GetOpenGLProcAddress() override
		{
			return SDL_GL_GetProcAddress;
		}

		void SetOpenGLHints() override
		{
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
			SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		}

		void SetOpenGLContext() override
		{
			_context = SDL_GL_CreateContext(_window);
			if (!_context)
			{
				LIB_LOGGER(error, window) << "sdl gl create context error: " + std::string(SDL_GetError());
				SDL_DestroyWindow(_window);
				SDL_Quit();
			}
		}

		void SetCustomTitleBar(const TitleBarHitTestDelegate& titleBarHitTestFunc) override
		{
			_titleBarHitTestFunc = titleBarHitTestFunc;

			SDL_SetWindowBordered(_window, false);
			SDL_SetWindowHitTest(_window,
				[](SDL_Window* window, const SDL_Point* area, void* data) -> SDL_HitTestResult
				{
					auto windowManager = static_cast<WindowManagerSDL*>(data);

					if ((SDL_GetWindowFlags(window) & SDL_WINDOW_MAXIMIZED) == 0)
					{
						const int border = 8;
						int width, height;
						SDL_GetWindowSize(window, &width, &height);

						bool left = area->x < border;
						bool right = area->x >= width - border;
						bool top = area->y < border;
						bool bottom = area->y >= height - border;

						if (top && left) return SDL_HITTEST_RESIZE_TOPLEFT;
						if (top && right) return SDL_HITTEST_RESIZE_TOPRIGHT;
						if (bottom && left) return SDL_HITTEST_RESIZE_BOTTOMLEFT;
						if (bottom && right) return SDL_HITTEST_RESIZE_BOTTOMRIGHT;
						if (left) return SDL_HITTEST_RESIZE_LEFT;
						if (right) return SDL_HITTEST_RESIZE_RIGHT;
						if (top) return SDL_HITTEST_RESIZE_TOP;
						if (bottom) return SDL_HITTEST_RESIZE_BOTTOM;
					}

					if (windowManager->_titleBarHitTestFunc && windowManager->_titleBarHitTestFunc(area->x, area->y))
					{
						return SDL_HITTEST_DRAGGABLE;
					}

					return SDL_HITTEST_NORMAL;
				},
				this
			);
		}

		void SetWindowIcon(int width, int height, const unsigned char* pixels) override
		{
			SDL_Surface* surface = SDL_CreateSurfaceFrom(width, height, SDL_PIXELFORMAT_RGBA32, const_cast<unsigned char*>(pixels), width * 4);
			if (surface)
			{
				SDL_SetWindowIcon(_window, surface);
				SDL_DestroySurface(surface);
			}
			else
			{
				LIB_LOGGER(error, window) << "sdl create surface from icon error: " + std::string(SDL_GetError());
			}
		}

		void IconifyWindow() override
		{
			SDL_MinimizeWindow(_window);
		}

		void MaximizeWindow() override
		{
			SDL_MaximizeWindow(_window);
		}

		void RestoreWindow() override
		{
			SDL_RestoreWindow(_window);
		}

		bool IsWindowMaximized() override
		{
			return (SDL_GetWindowFlags(_window) & SDL_WINDOW_MAXIMIZED) != 0;
		}

	private:
		void InitSDL()
		{
			if (!SDL_Init(SDL_INIT_VIDEO))
			{
				throw std::runtime_error("[window] sdl init error: " + std::string(SDL_GetError()));
			}

			LIB_LOGGER(info, window) << "sdl version '" << SDL_MAJOR_VERSION << "." << SDL_MINOR_VERSION << "." << SDL_MICRO_VERSION << "' init succeeded";
		}

		void CreateWindowSDL()
		{
			SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
			SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

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
				LIB_LOGGER(error, window) << "sdl window creating error: " + std::string(SDL_GetError());
				SDL_Quit();
			}

			LIB_LOGGER(info, window) << "sdl window created";
		}

		KeyCode GetKeyCodeEnum(int key)
		{
			switch (key)
			{
				case SDLK_A: return KeyCode::a;
				case SDLK_B: return KeyCode::b;
				case SDLK_C: return KeyCode::c;
				case SDLK_D: return KeyCode::d;
				case SDLK_E: return KeyCode::e;
				case SDLK_F: return KeyCode::f;
				case SDLK_G: return KeyCode::g;
				case SDLK_H: return KeyCode::h;
				case SDLK_I: return KeyCode::i;
				case SDLK_J: return KeyCode::j;
				case SDLK_K: return KeyCode::k;
				case SDLK_L: return KeyCode::l;
				case SDLK_M: return KeyCode::m;
				case SDLK_N: return KeyCode::n;
				case SDLK_O: return KeyCode::o;
				case SDLK_P: return KeyCode::p;
				case SDLK_Q: return KeyCode::q;
				case SDLK_R: return KeyCode::r;
				case SDLK_S: return KeyCode::s;
				case SDLK_T: return KeyCode::t;
				case SDLK_U: return KeyCode::u;
				case SDLK_V: return KeyCode::v;
				case SDLK_W: return KeyCode::w;
				case SDLK_X: return KeyCode::x;
				case SDLK_Y: return KeyCode::x;
				case SDLK_Z: return KeyCode::z;
				case SDLK_ESCAPE: return KeyCode::escape;
				default: return KeyCode::unknown;
			}
		}

		void FillModsVector(int mods, std::vector<InputModifier>& modsVector)
		{
			if (mods & SDL_KMOD_ALT)
			{
				modsVector.emplace_back(InputModifier::alt);
			}
			if (mods & SDL_KMOD_SHIFT)
			{
				modsVector.emplace_back(InputModifier::shift);
			}
			if (mods & SDL_KMOD_GUI)
			{
				modsVector.emplace_back(InputModifier::super);
			}
			if (mods & SDL_KMOD_CTRL)
			{
				modsVector.emplace_back(InputModifier::control);
			}
			if (mods & SDL_KMOD_NUM)
			{
				modsVector.emplace_back(InputModifier::num_lock);
			}
			if (mods & SDL_KMOD_CAPS)
			{
				modsVector.emplace_back(InputModifier::caps_lock);
			}
			if (modsVector.empty())
			{
				modsVector.emplace_back(InputModifier::noop);
			}
		}

		SDL_Window* _window{ nullptr };

		SDL_GLContext _context;

		bool _shouldClose{ false };

		TitleBarHitTestDelegate _titleBarHitTestFunc;

	};
}
