#pragma once
#pragma warning(disable: 4100)

#include "seri/window/IWindowManager.h"
#include "seri/renderer/AuxiliaryStructs.h"
#include "seri/input/InputManager.h"

#include <GLFW/glfw3.h>

#include <utility>
#include <stdexcept>

namespace seri
{
	class WindowsGLFWWindowManager : public IWindowManager
	{
	public:
		WindowsGLFWWindowManager() = default;

		~WindowsGLFWWindowManager() override
		{
			glfwDestroyWindow(_window);
			glfwTerminate();

			LOGGER(info, "[window] windows glfw window manager destroyed and terminated successfully");
		}

		void Init() override
		{
			if (_initialized)
			{
				throw std::runtime_error("[window] window manager is already initialized");
			}

			InitGLFW();

			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

			CreateGLFWWindow();
			SetContext();

			InitGlad();
			LogGLInfo();
			EnableDebugOutput();

			SetGLOptions();

			SetWindowUserPointer(static_cast<void*>(this));
			SetEventCallbacks();

			_initialized = true;

			LOGGER(info, "[window] glfw window manager created successfully");
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
					LOGGER(info, "[window] unexpected cursor mode");
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

		void SetWindowUserPointer(void* pointer)
		{
			glfwSetWindowUserPointer(_window, pointer);
		}

		void* GetWindowUserPointer()
		{
			return glfwGetWindowUserPointer(_window);
		}

	protected:
		void InitGlad() override
		{
			int version = gladLoadGL(glfwGetProcAddress);
			if (version == 0)
			{
				throw std::runtime_error("[window] glad load error");
			}

			LOGGER(info, "[window] loaded opengl " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version));
		}

	private:
		void InitGLFW()
		{
			if (!glfwInit())
			{
				throw std::runtime_error("[window] glfw init error");
			}

			LOGGER(info, "[window] gflw version '" << glfwGetVersionString() << "' init succeeded");
		}

		void CreateGLFWWindow()
		{
			if (_windowProperties.isFullscreen)
			{
				GLFWmonitor* glfwMonitor = glfwGetPrimaryMonitor();
				if (!glfwMonitor)
				{
					throw std::runtime_error("[window] getting glfw monitor failed");
				}

				int monXPos, monYPos;
				glfwGetMonitorWorkarea(glfwMonitor, &monXPos, &monYPos, &_windowProperties.windowWidth, &_windowProperties.windowHeight);
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

			LOGGER(info, "[window] glfw window created");
		}

		void SetContext()
		{
			glfwMakeContextCurrent(_window);
		}

		void SetEventCallbacks()
		{
			// input
			glfwSetKeyCallback(_window,
				[](GLFWwindow* window, int key, int scancode, int action, int mods)
				{
					if (auto windowManager = static_cast<WindowsGLFWWindowManager*>(glfwGetWindowUserPointer(window)))
					{
						KeyCode keyEnum = windowManager->GetKeyEnum(key);
						InputAction actionEnum = windowManager->GetInputActionEnum(action);
						std::vector<InputModifier> modsVector;
						windowManager->FillModsVector(mods, modsVector);

						if (keyEnum == KeyCode::unknown)
						{
							LOGGER(info, "[window] glfw event: key: unexpected key type");
							return;
						}

						windowManager->FireEvent(event::KeyEventData{ keyEnum, scancode, actionEnum, std::move(modsVector) });

						InputManager::RegisterKey(keyEnum, actionEnum);

						if (keyEnum == KeyCode::escape && actionEnum == InputAction::press)
						{
							windowManager->SetWindowShouldCloseToTrue();
						}
					}
				}
			);

			glfwSetCharCallback(_window,
				[](GLFWwindow* window, unsigned int codepoint)
				{
					if (auto windowManager = static_cast<WindowsGLFWWindowManager*>(glfwGetWindowUserPointer(window)))
					{
						windowManager->FireEvent(event::CharacterEventData{ codepoint });
					}
				}
			);

			glfwSetCharModsCallback(_window,
				[](GLFWwindow* window, unsigned int codepoint, int mods)
				{
					if (auto windowManager = static_cast<WindowsGLFWWindowManager*>(glfwGetWindowUserPointer(window)))
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
					if (auto windowManager = static_cast<WindowsGLFWWindowManager*>(glfwGetWindowUserPointer(window)))
					{
						windowManager->FireEvent(event::MouseEnterEventData{ entered ? true : false });
					}
				}
			);

			glfwSetMouseButtonCallback(_window,
				[](GLFWwindow* window, int button, int action, int mods)
				{
					if (auto windowManager = static_cast<WindowsGLFWWindowManager*>(glfwGetWindowUserPointer(window)))
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
					if (auto windowManager = static_cast<WindowsGLFWWindowManager*>(glfwGetWindowUserPointer(window)))
					{
						windowManager->FireEvent(event::MousePositionEventData{ xpos, ypos });

						InputManager::RegisterCursorPosition(xpos, ypos);
					}
				}
			);

			glfwSetScrollCallback(_window,
				[](GLFWwindow* window, double xoffset, double yoffset)
				{
					if (auto windowManager = static_cast<WindowsGLFWWindowManager*>(glfwGetWindowUserPointer(window)))
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
					if (auto windowManager = static_cast<WindowsGLFWWindowManager*>(glfwGetWindowUserPointer(window)))
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
					if (auto windowManager = static_cast<WindowsGLFWWindowManager*>(glfwGetWindowUserPointer(window)))
					{
						windowManager->FireEvent(event::WindowCloseEventData{});
					}
				}
			);

			glfwSetFramebufferSizeCallback(_window,
				[](GLFWwindow* window, int width, int height)
				{
					if (auto windowManager = static_cast<WindowsGLFWWindowManager*>(glfwGetWindowUserPointer(window)))
					{
						windowManager->FireEvent(event::WindowResizeEventData{ width, height });

						windowManager->SetViewport(0, 0, width, height);
					}
				}
			);

			glfwSetWindowPosCallback(_window,
				[](GLFWwindow* window, int xpos, int ypos)
				{
					if (auto windowManager = static_cast<WindowsGLFWWindowManager*>(glfwGetWindowUserPointer(window)))
					{
						//LOGGER(info, "[window] window new position: " << xpos << ", " << ypos);
					}
				}
			);

			glfwSetWindowSizeCallback(_window,
				[](GLFWwindow* window, int width, int height)
				{
					if (auto windowManager = static_cast<WindowsGLFWWindowManager*>(glfwGetWindowUserPointer(window)))
					{
						LOGGER(info, "[window] window new size: " << width << ", " << height);
					}
				}
			);

			glfwSetWindowRefreshCallback(_window,
				[](GLFWwindow* window)
				{
					if (auto windowManager = static_cast<WindowsGLFWWindowManager*>(glfwGetWindowUserPointer(window)))
					{
						//LOGGER(info, "[window] window refresh");
					}
				}
			);

			glfwSetWindowFocusCallback(_window,
				[](GLFWwindow* window, int focused)
				{
					if (auto windowManager = static_cast<WindowsGLFWWindowManager*>(glfwGetWindowUserPointer(window)))
					{
						LOGGER(info, "[window] window focus state: " << (focused ? "focused" : "not focused"));
					}
				}
			);

			glfwSetWindowIconifyCallback(_window,
				[](GLFWwindow* window, int iconified)
				{
					if (auto windowManager = static_cast<WindowsGLFWWindowManager*>(glfwGetWindowUserPointer(window)))
					{
						LOGGER(info, "[window] window iconify state: " << (iconified ? "iconified" : "not iconified"));
					}
				}
			);

			glfwSetWindowMaximizeCallback(_window,
				[](GLFWwindow* window, int maximized)
				{
					if (auto windowManager = static_cast<WindowsGLFWWindowManager*>(glfwGetWindowUserPointer(window)))
					{
						LOGGER(info, "[window] window maximize state: " << (maximized ? "maximized" : "not maximized"));
					}
				}
			);

			glfwSetWindowContentScaleCallback(_window,
				[](GLFWwindow* window, float xscale, float yscale)
				{
					if (auto windowManager = static_cast<WindowsGLFWWindowManager*>(glfwGetWindowUserPointer(window)))
					{
						LOGGER(info, "[window] window new scale: " << xscale << ", " << yscale);
					}
				}
			);

			// error
			glfwSetErrorCallback(
				[](int error, const char* description)
				{
					LOGGER(error, "[window] glfw error " << error << ": " << description);
				}
			);
		}

		void CheckGLError()
		{
			GLenum err;
			while ((err = glGetError()) != GL_NO_ERROR)
			{
				LOGGER(error, "[window] gl error occurred: " << err);
			}
		}

		void EnableDebugOutput()
		{
			static const auto getDebugSourceString = [](GLenum source)
				{
					if (source == GL_DEBUG_SOURCE_API)
					{
						return "Calls to the OpenGL API";
					}
					if (source == GL_DEBUG_SOURCE_WINDOW_SYSTEM)
					{
						return "Calls to a window-system API";
					}
					if (source == GL_DEBUG_SOURCE_SHADER_COMPILER)
					{
						return "A compiler for a shading language";
					}
					if (source == GL_DEBUG_SOURCE_THIRD_PARTY)
					{
						return "An application associated with OpenGL";
					}
					if (source == GL_DEBUG_SOURCE_APPLICATION)
					{
						return "Generated by the user of this application";
					}
					if (source == GL_DEBUG_SOURCE_OTHER)
					{
						return "Some source that isn't one of these";
					}

					return "Unknown source";
				};

			static const auto getDebugTypeString = [](GLenum type)
				{
					if (type == GL_DEBUG_TYPE_ERROR)
					{
						return "An error, typically from the API";
					}
					if (type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR)
					{
						return "Some behavior marked deprecated has been used";
					}
					if (type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR)
					{
						return "Something has invoked undefined behavior";
					}
					if (type == GL_DEBUG_TYPE_PORTABILITY)
					{
						return "Some functionality the user relies upon is not portable";
					}
					if (type == GL_DEBUG_TYPE_PERFORMANCE)
					{
						return "Code has triggered possible performance issues";
					}
					if (type == GL_DEBUG_TYPE_MARKER)
					{
						return "Command stream annotation";
					}
					if (type == GL_DEBUG_TYPE_PUSH_GROUP)
					{
						return "Group pushing";
					}
					if (type == GL_DEBUG_TYPE_POP_GROUP)
					{
						return "Group popping";
					}
					if (type == GL_DEBUG_TYPE_OTHER)
					{
						return "Some type that isn't one of these";
					}

					return "Unknown type";
				};

			static const auto getDebugSeverityString = [](GLenum severity)
				{
					if (severity == GL_DEBUG_SEVERITY_HIGH)
					{
						return "All OpenGL Errors, shader compilation / linking errors, or highly - dangerous undefined behavior";
					}
					else if (severity == GL_DEBUG_SEVERITY_MEDIUM)
					{
						return "Major performance warnings, shader compilation / linking warnings, or the use of deprecated functionality";
					}
					else if (severity == GL_DEBUG_SEVERITY_LOW)
					{
						return "Redundant state change performance warning, or unimportant undefined behavior";
					}
					else if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
					{
						return "Anything that isn't an error or performance issue";
					}

					return "Unknown source";
				};

			glEnable(GL_DEBUG_OUTPUT);
			glDebugMessageCallback(
				[](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
				{
					if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
					{
						return;
					}

					LOGGER(debug, "[window] gl debug message:" << "\n"
						<< "severity: " << getDebugSeverityString(severity) << "\n"
						<< "type: " << getDebugTypeString(type) << "\n"
						<< "source: " << getDebugSourceString(source) << "\n"
						<< "id: " << id << "\n"
						<< "message: " << message << "\n"
					);
				},
				nullptr
			);
		}

		void DisableDebugOutput()
		{
			glDisable(GL_DEBUG_OUTPUT);
		}

		KeyCode GetKeyEnum(int key)
		{
			switch (key)
			{
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
				case GLFW_KEY_Y: return KeyCode::x;
				case GLFW_KEY_Z: return KeyCode::z;
				case GLFW_KEY_ESCAPE: return KeyCode::escape;
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
