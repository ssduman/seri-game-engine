#pragma once

#include "seri/logging/Logger.h"
#include "seri/event/EventData.h"
#include "seri/event/EventCallback.h"

#include <memory>

namespace seri
{
	struct WindowProperties
	{
		const char* windowTitle = nullptr;
		bool isFullscreen = false;
		int windowWidth = 1280;
		int windowHeight = 720;
	};

	class IWindowManager
	{
	public:
		IWindowManager() = default;

		virtual ~IWindowManager() = default;

		virtual void init() = 0;

		virtual void Update() = 0;

		virtual double getTime() = 0;

		virtual void updateDeltaTime() = 0;

		virtual float getDeltaTime() = 0;

		virtual void* getWindow() = 0;

		virtual void hideCursor() = 0;

		virtual void enableCursor() = 0;

		virtual void disableCursor() = 0;

		virtual double getCursorX() = 0;

		virtual double getCursorY() = 0;

		virtual std::pair<double, double> getCursorPosition() = 0;

		virtual void setCursorPosition(double xpos, double ypos) = 0;

		virtual void viewport(int x, int y, int width, int height) = 0;

		virtual int windowShouldClose() = 0;

		virtual void clear() = 0;

		virtual void clearColor(float red = 0.2f, float green = 0.2f, float blue = 0.2f, float alpha = 1.0f) = 0;

		virtual void pollEvents() = 0;

		virtual void swapBuffers() = 0;

		virtual void setWindowShouldCloseToTrue() = 0;

		virtual void setPointSize(float size) = 0;

		virtual void setLineWidth(float width) = 0;

		virtual std::pair<int, int> getWindowPosition() = 0;

		virtual void setWindowPosition(int xpos, int ypos) = 0;

		virtual unsigned char* GetPixelsInScreen() = 0;

		virtual const char* getClipboard() = 0;

		virtual void setClipboard(const char* str) = 0;

		virtual void setWindowUserPointer(void* pointer) = 0;

		virtual void* getWindowUserPointer() = 0;

		virtual void fireEvent(event::IEventData& data) = 0;

		const std::shared_ptr<event::IEventCallback>& getEventCallback()
		{
			return _eventCallback;
		}

		void setEventCallback(std::shared_ptr<event::IEventCallback> eventCallback)
		{
			_eventCallback = std::move(eventCallback);
		}

		void setWindowProperties(WindowProperties windowProperties)
		{
			if (_initialized)
			{
				throw std::runtime_error("window is already initialized");
			}
			_windowProperties = std::move(windowProperties);
		}

		int getWidth()
		{
			return _windowProperties.windowWidth;
		}

		int getHeight()
		{
			return _windowProperties.windowHeight;
		}

		float getWidthF()
		{
			return static_cast<float>(_windowProperties.windowWidth);
		}

		float getHeightF()
		{
			return static_cast<float>(_windowProperties.windowHeight);
		}

		float getAspect()
		{
			return getWidthF() / getHeightF();
		}

	protected:
		WindowProperties _windowProperties;

		double _lastFrame{ 0.0 };
		double _deltaTime{ 0.0 };
		bool _initialized{ false };

		std::shared_ptr<event::IEventCallback> _eventCallback;

	};
}
