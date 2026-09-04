#pragma once

#include "seri/window/WindowManagerBase.h"

#include <memory>

namespace seri
{
	class WindowManager
	{
	public:
		static std::unique_ptr<WindowManagerBase>& Instance()
		{
			return _windowManager;
		}

		static void* GetWindowHandle()
		{
			return _windowManager->GetWindowHandle();
		}

		static void* GetContext()
		{
			return _windowManager->GetContext();
		}

		static double GetTime()
		{
			return _windowManager->GetTime();
		}

		static void SetVSyncCount(int count)
		{
			_windowManager->SetVSyncCount(count);
		}

		static void PollEvents()
		{
			_windowManager->PollEvents();
		}

		static void SwapBuffers()
		{
			_windowManager->SwapBuffers();
		}

		static bool GetWindowShouldClose()
		{
			return _windowManager->GetWindowShouldClose();
		}

		static void SetWindowShouldCloseToTrue()
		{
			_windowManager->SetWindowShouldCloseToTrue();
		}

		static int GetWidth()
		{
			return _windowManager->GetWindowProperties().windowWidth;
		}

		static int GetHeight()
		{
			return _windowManager->GetWindowProperties().windowHeight;
		}

		static float GetAspectRatio()
		{
			return (float)GetWidth() / (float)GetHeight();
		}

		static void SetCursorMode(CursorMode cursorMode)
		{
			_windowManager->SetCursorMode(cursorMode);
		}

		static void SetCustomTitleBar(const TitleBarHitTestDelegate& titleBarHitTestFunc)
		{
			_windowManager->SetCustomTitleBar(titleBarHitTestFunc);
		}

		static void SetWindowIcon(int width, int height, const unsigned char* pixels)
		{
			_windowManager->SetWindowIcon(width, height, pixels);
		}

		static void IconifyWindow()
		{
			_windowManager->IconifyWindow();
		}

		static void MaximizeWindow()
		{
			_windowManager->MaximizeWindow();
		}

		static void RestoreWindow()
		{
			_windowManager->RestoreWindow();
		}

		static bool IsWindowMaximized()
		{
			return _windowManager->IsWindowMaximized();
		}

		static void FireEvent(const event::IEventData& data)
		{
			_windowManager->FireEvent(data);
		}

	private:
		static std::unique_ptr<WindowManagerBase> _windowManager;

	};
}
