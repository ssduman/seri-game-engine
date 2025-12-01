#pragma once

#include "seri/logging/Logger.h"
#include "seri/event/EventData.h"
#include "seri/event/EventCallback.h"
#include "seri/core/TimeWrapper.h"

#include <memory>
#include <functional>

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

	class WindowManagerBase
	{
	public:
		WindowManagerBase() = default;

		virtual ~WindowManagerBase() = default;

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

		virtual void* GetOpenGLProcAddress() = 0;

		virtual void SetOpenGLHints() = 0;

		virtual void SetOpenGLContext() = 0;

		WindowProperties GetWindowProperties()
		{
			return _windowProperties;
		}

		void SetWindowProperties(WindowProperties windowProperties)
		{
			if (_initialized)
			{
				throw std::runtime_error("[window] already initialized");
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

		float GetAspectRatio()
		{
			return (float)GetWidth() / (float)GetHeight();
		}

		void FireEvent(const event::IEventData& data)
		{
			for (const auto& _eventCallback : _eventCallbacks)
			{
				if (_eventCallback)
				{
					_eventCallback->FireEvent(data);
				}
			}
		};

		void AddEventCallback(std::shared_ptr<event::IEventCallback> eventCallback)
		{
			_eventCallbacks.emplace_back(std::move(eventCallback));
		}

		void AddProcessEventDelegate(const ProcessEventDelegate& delegate)
		{
			processEventDelegateList.emplace_back(delegate);
		}

	protected:
		void SetViewport(int x, int y, int width, int height)
		{
			_windowProperties.windowWidth = width;
			_windowProperties.windowHeight = height;
		}

		WindowProperties _windowProperties;

		bool _initialized{ false };

		std::vector<std::shared_ptr<event::IEventCallback>> _eventCallbacks;

		std::vector<ProcessEventDelegate> processEventDelegateList;

	};
}
