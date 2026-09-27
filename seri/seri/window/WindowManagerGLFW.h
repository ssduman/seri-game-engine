#pragma once

#include "seri/core/Core.h"
#include "seri/input/InputManager.h"
#include "seri/window/WindowManagerBase.h"
#include "seri/platform/Platform.h"

#if defined (SERI_WINDOWS)
#define GLFW_EXPOSE_NATIVE_WIN32
#endif

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <utility>
#include <stdexcept>

namespace seri
{
	class WindowManagerGLFW : public WindowManagerBase
	{
	public:
		WindowManagerGLFW() = default;

		~WindowManagerGLFW() override
		{
			glfwDestroyWindow(_window);
			glfwTerminate();

			LIB_LOGGER(info, window) << "glfw window manager destroyed and terminated successfully";
		}

		void Init(WindowProperties windowProperties) override
		{
			if (_initialized)
			{
				throw std::runtime_error("[window] glfw window manager is already initialized");
			}

			_windowProperties = windowProperties;

			InitGLFW();
			CreateWindowGLFW();
			SetWindowUserPointer(static_cast<void*>(this));
			SetWindowEventCallbacks();

			_initialized = true;

			LIB_LOGGER(info, window) << "glfw window manager created successfully";
		}

		double GetTime() override
		{
			return glfwGetTime();
		}

		void* GetWindowHandle() override
		{
			return _window;
		}

		void* GetContext() override
		{
			return glfwGetCurrentContext();
		}

		void SetCursorMode(CursorMode cursorMode) override
		{
			switch (cursorMode)
			{
				case seri::normal:
					glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
					break;
				case seri::hidden:
					glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
					break;
				case seri::disabled:
					glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
					break;
				default:
					LIB_LOGGER(info, window) << "unexpected cursor mode";
					break;
			}
		}

		std::pair<double, double> GetCursorPosition() override
		{
			double mouseXPosition, mouseYPosition;
			glfwGetCursorPos(_window, &mouseXPosition, &mouseYPosition);
			return { mouseXPosition, mouseYPosition };
		}

		void SetCursorPosition(double xpos, double ypos) override
		{
			glfwSetCursorPos(_window, xpos, ypos);
		}

		void SetVSyncCount(int count) override
		{
			glfwSwapInterval(count);
		}

		void PollEvents() override
		{
			glfwPollEvents();
		}

		void SwapBuffers() override
		{
			glfwSwapBuffers(_window);
		}

		bool GetWindowShouldClose() override
		{
			return glfwWindowShouldClose(_window) > 0;
		}

		void SetWindowShouldCloseToTrue() override
		{
			glfwSetWindowShouldClose(_window, GLFW_TRUE);
		}

		std::pair<int, int> GetWindowPosition() override
		{
			int xpos, ypos;
			glfwGetWindowPos(_window, &xpos, &ypos);
			return { xpos, ypos };
		}

		void SetWindowPosition(int xpos, int ypos) override
		{
			glfwSetWindowPos(_window, xpos, ypos);
		}

		const char* GetClipboard() override
		{
			return glfwGetClipboardString(nullptr);
		}

		void SetClipboard(const char* str) override
		{
			glfwSetClipboardString(nullptr, str);
		}

		void SetWindowUserPointer(void* pointer) override
		{
			glfwSetWindowUserPointer(_window, pointer);
		}

		void* GetWindowUserPointer() override
		{
			return glfwGetWindowUserPointer(_window);
		}

		void* GetOpenGLProcAddress() override
		{
			return glfwGetProcAddress;
		}

		void SetOpenGLHints() override
		{
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		}

		void SetOpenGLContext() override
		{
			glfwMakeContextCurrent(_window);
		}

		void SetCustomTitleBar(const TitleBarHitTestDelegate& titleBarHitTestFunc) override
		{
			platform::EnableCustomTitleBar(GetNativeWindowHandle(), titleBarHitTestFunc);
		}

		void SetWindowIcon(int width, int height, const unsigned char* pixels) override
		{
			GLFWimage image{ width, height, const_cast<unsigned char*>(pixels) };
			glfwSetWindowIcon(_window, 1, &image);
		}

		void IconifyWindow() override
		{
			glfwIconifyWindow(_window);
		}

		void MaximizeWindow() override
		{
			glfwMaximizeWindow(_window);
		}

		void RestoreWindow() override
		{
			glfwRestoreWindow(_window);
		}

		void SetWindowSize(int width, int height) override
		{
			glfwSetWindowSize(_window, width, height);
		}

		void SetWindowTitle(const char* title) override
		{
			glfwSetWindowTitle(_window, title);
		}

		bool IsWindowMaximized() override
		{
			return glfwGetWindowAttrib(_window, GLFW_MAXIMIZED) == GLFW_TRUE;
		}

	private:
		void* GetNativeWindowHandle()
		{
#if defined (SERI_WINDOWS)
			return glfwGetWin32Window(_window);
#else
			static_assert(false, "native window handle not supported on this platform");
#endif
		}

		void InitGLFW()
		{
			if (!glfwInit())
			{
				throw std::runtime_error("[window] glfw init error");
			}

			LIB_LOGGER(info, window) << "gflw version '" << glfwGetVersionString() << "' init succeeded";
		}

		void CreateWindowGLFW()
		{
			SetOpenGLHints();

			glfwWindowHint(GLFW_DEPTH_BITS, 24);
			glfwWindowHint(GLFW_STENCIL_BITS, 8);

			if (_windowProperties.isFullscreen)
			{
				GLFWmonitor* glfwMonitor = glfwGetPrimaryMonitor();
				if (!glfwMonitor)
				{
					throw std::runtime_error("[window] getting glfw monitor failed");
				}

				const GLFWvidmode* videoMode = glfwGetVideoMode(glfwMonitor);
				if (!videoMode)
				{
					throw std::runtime_error("[window] getting glfw video mode failed");
				}

				glfwWindowHint(GLFW_RED_BITS, videoMode->redBits);
				glfwWindowHint(GLFW_GREEN_BITS, videoMode->greenBits);
				glfwWindowHint(GLFW_BLUE_BITS, videoMode->blueBits);
				glfwWindowHint(GLFW_REFRESH_RATE, videoMode->refreshRate);

				_windowProperties.windowWidth = videoMode->width;
				_windowProperties.windowHeight = videoMode->height;
				_window = glfwCreateWindow(_windowProperties.windowWidth, _windowProperties.windowHeight, _windowProperties.windowTitle, glfwMonitor, nullptr);
			}
			else
			{
				_window = glfwCreateWindow(_windowProperties.windowWidth, _windowProperties.windowHeight, _windowProperties.windowTitle, nullptr, nullptr);
			}

			if (!_window)
			{
				throw std::runtime_error("[window] glfw window creating error");
			}

			LIB_LOGGER(info, window) << "glfw window created";
		}

		void SetWindowEventCallbacks()
		{
			// input
			glfwSetKeyCallback(_window,
				[](GLFWwindow* window, int key, int scancode, int action, int mods)
				{
					if (auto windowManager = static_cast<WindowManagerGLFW*>(glfwGetWindowUserPointer(window)))
					{
						KeyCode keyEnum = windowManager->GetKeyCodeEnum(key);
						InputAction actionEnum = windowManager->GetInputActionEnum(action);
						std::vector<InputModifier> modsVector;
						windowManager->FillModsVector(mods, modsVector);

						if (keyEnum == KeyCode::unknown)
						{
							//LIB_LOGGER(info, window) << "glfw event: key: unexpected key type";
							return;
						}

						windowManager->FireEvent(event::KeyEventData{ keyEnum, scancode, actionEnum, std::move(modsVector) });

						InputManager::RegisterKey(keyEnum, actionEnum);
					}
				}
			);

			glfwSetCharCallback(_window,
				[](GLFWwindow* window, unsigned int codepoint)
				{
					if (auto windowManager = static_cast<WindowManagerGLFW*>(glfwGetWindowUserPointer(window)))
					{
						windowManager->FireEvent(event::CharacterEventData{ codepoint });
					}
				}
			);

			glfwSetCharModsCallback(_window,
				[](GLFWwindow* window, unsigned int codepoint, int mods)
				{
					if (auto windowManager = static_cast<WindowManagerGLFW*>(glfwGetWindowUserPointer(window)))
					{
						std::vector<InputModifier> modsVector;
						windowManager->FillModsVector(mods, modsVector);

						windowManager->FireEvent(event::CharacterModsEventData{ codepoint, std::move(modsVector) });
					}
				}
			);

			// mouse
			glfwSetCursorEnterCallback(_window,
				[](GLFWwindow* window, int entered)
				{
					if (auto windowManager = static_cast<WindowManagerGLFW*>(glfwGetWindowUserPointer(window)))
					{
						windowManager->FireEvent(event::MouseEnterEventData{ entered ? true : false });
					}
				}
			);

			glfwSetMouseButtonCallback(_window,
				[](GLFWwindow* window, int button, int action, int mods)
				{
					if (auto windowManager = static_cast<WindowManagerGLFW*>(glfwGetWindowUserPointer(window)))
					{
						MouseButtonCode buttonEnum = windowManager->GetMouseButtonEnum(button);
						InputAction actionEnum = windowManager->GetInputActionEnum(action);
						InputModifier modsEnum = InputModifier::noop;

						windowManager->FireEvent(event::MouseButtonEventData{ buttonEnum, actionEnum, modsEnum });

						InputManager::RegisterMouse(buttonEnum, actionEnum);
					}
				}
			);

			glfwSetCursorPosCallback(_window,
				[](GLFWwindow* window, double xpos, double ypos)
				{
					if (auto windowManager = static_cast<WindowManagerGLFW*>(glfwGetWindowUserPointer(window)))
					{
						windowManager->FireEvent(event::MousePositionEventData{ xpos, ypos });

						InputManager::RegisterCursorPosition(xpos, ypos);
					}
				}
			);

			glfwSetScrollCallback(_window,
				[](GLFWwindow* window, double xoffset, double yoffset)
				{
					if (auto windowManager = static_cast<WindowManagerGLFW*>(glfwGetWindowUserPointer(window)))
					{
						windowManager->FireEvent(event::MouseScrollEventData{ xoffset, yoffset });

						InputManager::RegisterScrollDelta(xoffset, yoffset);
					}
				}
			);

			// window
			glfwSetDropCallback(_window,
				[](GLFWwindow* window, int path_count, const char* paths[])
				{
					if (auto windowManager = static_cast<WindowManagerGLFW*>(glfwGetWindowUserPointer(window)))
					{
						std::vector<std::string> pathVector;
						for (int i = 0; i < path_count; i++)
						{
							pathVector.emplace_back(paths[i]);
						}

						windowManager->FireEvent(event::WindowDropEventData{ std::move(pathVector) });
					}
				}
			);

			glfwSetWindowCloseCallback(_window,
				[](GLFWwindow* window)
				{
					if (auto windowManager = static_cast<WindowManagerGLFW*>(glfwGetWindowUserPointer(window)))
					{
						windowManager->FireEvent(event::WindowCloseEventData{});
					}
				}
			);

			glfwSetFramebufferSizeCallback(_window,
				[](GLFWwindow* window, int width, int height)
				{
					if (auto windowManager = static_cast<WindowManagerGLFW*>(glfwGetWindowUserPointer(window)))
					{
						windowManager->FireEvent(event::WindowResizeEventData{ width, height });

						windowManager->SetViewport(0, 0, width, height);
					}
				}
			);

			glfwSetWindowPosCallback(_window,
				[](GLFWwindow* window, int xpos, int ypos)
				{
					if (auto windowManager = static_cast<WindowManagerGLFW*>(glfwGetWindowUserPointer(window)))
					{
						//LIB_LOGGER(info, window) << "window new position: " << xpos << ", " << ypos;
					}
				}
			);

			glfwSetWindowSizeCallback(_window,
				[](GLFWwindow* window, int width, int height)
				{
					if (auto windowManager = static_cast<WindowManagerGLFW*>(glfwGetWindowUserPointer(window)))
					{
						//LIB_LOGGER(info, window) << "window new size: " << width << ", " << height;
					}
				}
			);

			glfwSetWindowRefreshCallback(_window,
				[](GLFWwindow* window)
				{
					if (auto windowManager = static_cast<WindowManagerGLFW*>(glfwGetWindowUserPointer(window)))
					{
						//LIB_LOGGER(info, window) << "window refresh";
					}
				}
			);

			glfwSetWindowFocusCallback(_window,
				[](GLFWwindow* window, int focused)
				{
					if (auto windowManager = static_cast<WindowManagerGLFW*>(glfwGetWindowUserPointer(window)))
					{
						//LIB_LOGGER(info, window) << "window focus state: " << (focused ? "focused" : "not focused");
					}
				}
			);

			glfwSetWindowIconifyCallback(_window,
				[](GLFWwindow* window, int iconified)
				{
					if (auto windowManager = static_cast<WindowManagerGLFW*>(glfwGetWindowUserPointer(window)))
					{
						LIB_LOGGER(info, window) << "window iconify state: " << (iconified ? "iconified" : "not iconified");
					}
				}
			);

			glfwSetWindowMaximizeCallback(_window,
				[](GLFWwindow* window, int maximized)
				{
					if (auto windowManager = static_cast<WindowManagerGLFW*>(glfwGetWindowUserPointer(window)))
					{
						LIB_LOGGER(info, window) << "window maximize state: " << (maximized ? "maximized" : "not maximized");
					}
				}
			);

			glfwSetWindowContentScaleCallback(_window,
				[](GLFWwindow* window, float xscale, float yscale)
				{
					if (auto windowManager = static_cast<WindowManagerGLFW*>(glfwGetWindowUserPointer(window)))
					{
						LIB_LOGGER(info, window) << "window new scale: " << xscale << ", " << yscale;
					}
				}
			);

			// error
			glfwSetErrorCallback(
				[](int error, const char* description)
				{
					LIB_LOGGER(error, window) << "glfw error " << error << ": " << description;
				}
			);
		}

		KeyCode GetKeyCodeEnum(int key)
		{
			switch (key)
			{
				case GLFW_KEY_UP: return KeyCode::up;
				case GLFW_KEY_DOWN: return KeyCode::down;
				case GLFW_KEY_LEFT: return KeyCode::left;
				case GLFW_KEY_RIGHT: return KeyCode::right;
				case GLFW_KEY_0: return KeyCode::number_0;
				case GLFW_KEY_1: return KeyCode::number_1;
				case GLFW_KEY_2: return KeyCode::number_2;
				case GLFW_KEY_3: return KeyCode::number_3;
				case GLFW_KEY_4: return KeyCode::number_4;
				case GLFW_KEY_5: return KeyCode::number_5;
				case GLFW_KEY_6: return KeyCode::number_6;
				case GLFW_KEY_7: return KeyCode::number_7;
				case GLFW_KEY_8: return KeyCode::number_8;
				case GLFW_KEY_9: return KeyCode::number_9;
				case GLFW_KEY_A: return KeyCode::a;
				case GLFW_KEY_B: return KeyCode::b;
				case GLFW_KEY_C: return KeyCode::c;
				case GLFW_KEY_D: return KeyCode::d;
				case GLFW_KEY_E: return KeyCode::e;
				case GLFW_KEY_F: return KeyCode::f;
				case GLFW_KEY_G: return KeyCode::g;
				case GLFW_KEY_H: return KeyCode::h;
				case GLFW_KEY_I: return KeyCode::i;
				case GLFW_KEY_J: return KeyCode::j;
				case GLFW_KEY_K: return KeyCode::k;
				case GLFW_KEY_L: return KeyCode::l;
				case GLFW_KEY_M: return KeyCode::m;
				case GLFW_KEY_N: return KeyCode::n;
				case GLFW_KEY_O: return KeyCode::o;
				case GLFW_KEY_P: return KeyCode::p;
				case GLFW_KEY_Q: return KeyCode::q;
				case GLFW_KEY_R: return KeyCode::r;
				case GLFW_KEY_S: return KeyCode::s;
				case GLFW_KEY_T: return KeyCode::t;
				case GLFW_KEY_U: return KeyCode::u;
				case GLFW_KEY_V: return KeyCode::v;
				case GLFW_KEY_W: return KeyCode::w;
				case GLFW_KEY_X: return KeyCode::x;
				case GLFW_KEY_Y: return KeyCode::y;
				case GLFW_KEY_Z: return KeyCode::z;
				case GLFW_KEY_COMMA: return KeyCode::comma;
				case GLFW_KEY_EQUAL: return KeyCode::equal;
				case GLFW_KEY_MINUS: return KeyCode::minus;
				case GLFW_KEY_SLASH: return KeyCode::slash;
				case GLFW_KEY_PERIOD: return KeyCode::period;
				case GLFW_KEY_SEMICOLON: return KeyCode::semicolon;
				case GLFW_KEY_BACKSLASH: return KeyCode::backslash;
				case GLFW_KEY_APOSTROPHE: return KeyCode::apostrophe;
				case GLFW_KEY_GRAVE_ACCENT: return KeyCode::grave_accent;
				case GLFW_KEY_LEFT_BRACKET: return KeyCode::left_bracket;
				case GLFW_KEY_RIGHT_BRACKET: return KeyCode::right_bracket;
				case GLFW_KEY_END: return KeyCode::end;
				case GLFW_KEY_TAB: return KeyCode::tab;
				case GLFW_KEY_HOME: return KeyCode::home;
				case GLFW_KEY_MENU: return KeyCode::menu;
				case GLFW_KEY_DELETE: return KeyCode::del;
				case GLFW_KEY_PAUSE: return KeyCode::pause;
				case GLFW_KEY_ENTER: return KeyCode::enter;
				case GLFW_KEY_SPACE: return KeyCode::space;
				case GLFW_KEY_INSERT: return KeyCode::insert;
				case GLFW_KEY_ESCAPE: return KeyCode::escape;
				case GLFW_KEY_PAGE_UP: return KeyCode::page_up;
				case GLFW_KEY_NUM_LOCK: return KeyCode::num_lock;
				case GLFW_KEY_PAGE_DOWN: return KeyCode::page_down;
				case GLFW_KEY_CAPS_LOCK: return KeyCode::caps_lock;
				case GLFW_KEY_BACKSPACE: return KeyCode::backspace;
				case GLFW_KEY_SCROLL_LOCK: return KeyCode::scroll_lock;
				case GLFW_KEY_PRINT_SCREEN: return KeyCode::print_screen;
				case GLFW_KEY_LEFT_ALT: return KeyCode::left_alt;
				case GLFW_KEY_LEFT_SHIFT: return KeyCode::left_shift;
				case GLFW_KEY_LEFT_SUPER: return KeyCode::left_super;
				case GLFW_KEY_LEFT_CONTROL: return KeyCode::left_control;
				case GLFW_KEY_RIGHT_ALT: return KeyCode::right_alt;
				case GLFW_KEY_RIGHT_SHIFT: return KeyCode::right_shift;
				case GLFW_KEY_RIGHT_SUPER: return KeyCode::right_super;
				case GLFW_KEY_RIGHT_CONTROL: return KeyCode::right_control;
				case GLFW_KEY_F1: return KeyCode::f1;
				case GLFW_KEY_F2: return KeyCode::f2;
				case GLFW_KEY_F3: return KeyCode::f3;
				case GLFW_KEY_F4: return KeyCode::f4;
				case GLFW_KEY_F5: return KeyCode::f5;
				case GLFW_KEY_F6: return KeyCode::f6;
				case GLFW_KEY_F7: return KeyCode::f7;
				case GLFW_KEY_F8: return KeyCode::f8;
				case GLFW_KEY_F9: return KeyCode::f9;
				case GLFW_KEY_F10: return KeyCode::f10;
				case GLFW_KEY_F11: return KeyCode::f11;
				case GLFW_KEY_F12: return KeyCode::f12;
				case GLFW_KEY_F13: return KeyCode::f13;
				case GLFW_KEY_F14: return KeyCode::f14;
				case GLFW_KEY_F15: return KeyCode::f15;
				case GLFW_KEY_F16: return KeyCode::f16;
				case GLFW_KEY_F17: return KeyCode::f17;
				case GLFW_KEY_F18: return KeyCode::f18;
				case GLFW_KEY_F19: return KeyCode::f19;
				case GLFW_KEY_F20: return KeyCode::f20;
				case GLFW_KEY_F21: return KeyCode::f21;
				case GLFW_KEY_F22: return KeyCode::f22;
				case GLFW_KEY_F23: return KeyCode::f23;
				case GLFW_KEY_F24: return KeyCode::f24;
				case GLFW_KEY_KP_0: return KeyCode::kp_0;
				case GLFW_KEY_KP_1: return KeyCode::kp_1;
				case GLFW_KEY_KP_2: return KeyCode::kp_2;
				case GLFW_KEY_KP_3: return KeyCode::kp_3;
				case GLFW_KEY_KP_4: return KeyCode::kp_4;
				case GLFW_KEY_KP_5: return KeyCode::kp_5;
				case GLFW_KEY_KP_6: return KeyCode::kp_6;
				case GLFW_KEY_KP_7: return KeyCode::kp_7;
				case GLFW_KEY_KP_8: return KeyCode::kp_8;
				case GLFW_KEY_KP_9: return KeyCode::kp_9;
				case GLFW_KEY_KP_ADD: return KeyCode::kp_add;
				case GLFW_KEY_KP_ENTER: return KeyCode::kp_enter;
				case GLFW_KEY_KP_EQUAL: return KeyCode::kp_equal;
				case GLFW_KEY_KP_DIVIDE: return KeyCode::kp_divide;
				case GLFW_KEY_KP_DECIMAL: return KeyCode::kp_decimal;
				case GLFW_KEY_KP_MULTIPLY: return KeyCode::kp_multiply;
				case GLFW_KEY_KP_SUBTRACT: return KeyCode::kp_subtract;
				default: return KeyCode::unknown;
			}
		}

		InputAction GetInputActionEnum(int action)
		{
			switch (action)
			{
				case GLFW_PRESS: return InputAction::press;
				case GLFW_RELEASE: return InputAction::release;
				case GLFW_REPEAT: return InputAction::repeat;
				default: return InputAction::noop;
			}
		}

		MouseButtonCode GetMouseButtonEnum(int button)
		{
			switch (button)
			{
				case GLFW_MOUSE_BUTTON_LEFT: return MouseButtonCode::button_left;
				case GLFW_MOUSE_BUTTON_MIDDLE: return MouseButtonCode::button_middle;
				case GLFW_MOUSE_BUTTON_RIGHT: return MouseButtonCode::button_right;
				case GLFW_MOUSE_BUTTON_4: return MouseButtonCode::button_4;
				case GLFW_MOUSE_BUTTON_5: return MouseButtonCode::button_5;
				default: return MouseButtonCode::noop;
			}
		}

		void FillModsVector(int mods, std::vector<InputModifier>& modsVector)
		{
			if (mods & GLFW_MOD_ALT)
			{
				modsVector.emplace_back(InputModifier::alt);
			}
			if (mods & GLFW_MOD_SHIFT)
			{
				modsVector.emplace_back(InputModifier::shift);
			}
			if (mods & GLFW_MOD_SUPER)
			{
				modsVector.emplace_back(InputModifier::super);
			}
			if (mods & GLFW_MOD_CONTROL)
			{
				modsVector.emplace_back(InputModifier::control);
			}
			if (mods & GLFW_MOD_NUM_LOCK)
			{
				modsVector.emplace_back(InputModifier::num_lock);
			}
			if (mods & GLFW_MOD_CAPS_LOCK)
			{
				modsVector.emplace_back(InputModifier::caps_lock);
			}
			if (modsVector.empty())
			{
				modsVector.emplace_back(InputModifier::noop);
			}
		}

		GLFWwindow* _window{ nullptr };

	};
}
