#pragma once

#include "seri/logging/Logger.h"
#include "seri/event/EventData.h"
#include "seri/event/EventCallback.h"
#include "seri/core/TimeWrapper.h"

#include <memory>
#include <functional>

#include <glad/gl.h>

namespace seri
{
	struct WindowProperties
	{
		const char* windowTitle = nullptr;
		bool isFullscreen = false;
		int windowWidth = 1280;
		int windowHeight = 720;
	};

	enum CursorMode
	{
		normal = 0,
		hidden = 1,
		disabled = 2,
	};

	using ProcessEventDelegate = std::function<void(const void*)>;

	class IWindowManager
	{
	public:
		IWindowManager() = default;

		virtual ~IWindowManager() = default;

		virtual void Init() = 0;

		virtual double GetTime() = 0;

		virtual void* GetWindowHandle() = 0;

		virtual void* GetContext() = 0;

		virtual void SetVSyncCount(int count) = 0;

		virtual void PollEvents() = 0;

		virtual void SwapBuffers() = 0;

		virtual bool GetWindowShouldClose() = 0;

		virtual void SetWindowShouldCloseToTrue() = 0;

		virtual void SetCursorMode(CursorMode cursorMode) = 0;

		virtual std::pair<double, double> GetCursorPosition() = 0;

		virtual void SetCursorPosition(double xpos, double ypos) = 0;

		virtual std::pair<int, int> GetWindowPosition() = 0;

		virtual void SetWindowPosition(int xpos, int ypos) = 0;

		virtual const char* GetClipboard() = 0;

		virtual void SetClipboard(const char* str) = 0;

		virtual void SetWindowUserPointer(void* pointer) = 0;

		virtual void* GetWindowUserPointer() = 0;

		void UpdateDeltaTime()
		{
			auto currentFrame = GetTime();
			_deltaTime = currentFrame - _lastFrame;
			_lastFrame = currentFrame;

			TimeWrapper::RegisterTime(static_cast<float>(currentFrame), static_cast<float>(_deltaTime));
		}

		float GetDeltaTime()
		{
			return static_cast<float>(_deltaTime);
		}

		void SetEventCallback(std::shared_ptr<event::IEventCallback> eventCallback)
		{
			_eventCallback = std::move(eventCallback);
		}

		void SetWindowProperties(WindowProperties windowProperties)
		{
			if (_initialized)
			{
				throw std::runtime_error("window is already initialized");
			}
			_windowProperties = std::move(windowProperties);
		}

		int GetWidth()
		{
			return _windowProperties.windowWidth;
		}

		int GetHeight()
		{
			return _windowProperties.windowHeight;
		}

		float GetWidthF()
		{
			return static_cast<float>(_windowProperties.windowWidth);
		}

		float GetHeightF()
		{
			return static_cast<float>(_windowProperties.windowHeight);
		}

		float GetAspectRatio()
		{
			return GetWidthF() / GetHeightF();
		}

		void FireEvent(event::IEventData& data)
		{
			_eventCallback->FireEvent(data);
		};

		void AddProcessEventDelegate(const ProcessEventDelegate& delegate)
		{
			processEventDelegateList.emplace_back(delegate);
		}

		void SetViewport(int x, int y, int width, int height)
		{
			_windowProperties.windowWidth = width;
			_windowProperties.windowHeight = height;
			glViewport(x, y, _windowProperties.windowWidth, _windowProperties.windowHeight);
		}

		void Clear()
		{
			const unsigned int mask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
			glClear(mask);
		}

		void ClearColor(float red = 0.2f, float green = 0.2f, float blue = 0.2f, float alpha = 1.0f)
		{
			glClearColor(red, green, blue, alpha);
		}

		void SetPointSize(float size)
		{
			glPointSize(size);
		}

		void SetLineWidth(float width)
		{
			glLineWidth(width);
		}

		unsigned char* GetPixelsInScreen()
		{
			int width = GetWidth();
			int height = GetHeight();

			unsigned char* pixels = new unsigned char[width * height * 4];

			glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

			for (int y = 0; y < height / 2; ++y)
			{
				int oppositeY = height - 1 - y;
				for (int x = 0; x < width * 4; ++x)
				{
					std::swap(pixels[y * width * 4 + x], pixels[oppositeY * width * 4 + x]);
				}
			}

			return pixels;
		}

	protected:
		virtual void InitOpenGLGlad() = 0;

		virtual void SetOpenGLHints() = 0;

		virtual void SetOpenGLContext() = 0;

		void SetOpenGLOptions()
		{
			glEnable(GL_BLEND);
			glEnable(GL_DEPTH_TEST);
			//glEnable(GL_FRAMEBUFFER_SRGB);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}

		void LogOpenGLInfo()
		{
			LOGGER(info, "[window] vendor: " << glGetString(GL_VENDOR));
			LOGGER(info, "[window] version: " << glGetString(GL_VERSION));
			LOGGER(info, "[window] renderer: " << glGetString(GL_RENDERER));
			LOGGER(info, "[window] shading language version: " << glGetString(GL_SHADING_LANGUAGE_VERSION));
		}

		void CheckOpenGLError()
		{
			GLenum err;
			while ((err = glGetError()) != GL_NO_ERROR)
			{
				LOGGER(error, "[window] gl error occurred: " << err);
			}
		}

		void EnableOpenGLDebugOutput()
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

		void DisableOpenGLDebugOutput()
		{
			glDisable(GL_DEBUG_OUTPUT);
		}

		WindowProperties _windowProperties;

		double _lastFrame{ 0.0 };
		double _deltaTime{ 0.0 };
		bool _initialized{ false };

		std::shared_ptr<event::IEventCallback> _eventCallback;

		std::vector<ProcessEventDelegate> processEventDelegateList;

	};
}
