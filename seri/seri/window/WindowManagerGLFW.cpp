#include "Seripch.h"

#include "seri/core/Seri.h"

#if defined (SERI_USE_WINDOW_GLFW)

#include "seri/window/WindowManagerGLFW.h"

#include <windows.h>
#include <dwmapi.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "dwmapi.lib")

namespace seri
{
	struct NativeTitleBarState
	{
		WindowManagerGLFW* windowManager{ nullptr };
		WNDPROC prevWindowProc{ nullptr };
	};

	static void GetNativeFrameSize(HWND hwnd, int& frameX, int& frameY)
	{
		UINT dpi = GetDpiForWindow(hwnd);
		int paddedBorder = GetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi);
		frameX = GetSystemMetricsForDpi(SM_CXFRAME, dpi) + paddedBorder;
		frameY = GetSystemMetricsForDpi(SM_CYFRAME, dpi) + paddedBorder;
	}

	static LRESULT CALLBACK TitleBarWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		auto state = reinterpret_cast<NativeTitleBarState*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
		if (!state)
		{
			return DefWindowProcW(hwnd, msg, wParam, lParam);
		}

		switch (msg)
		{
			case WM_NCCALCSIZE:
			{
				if (IsZoomed(hwnd))
				{
					int frameX, frameY;
					GetNativeFrameSize(hwnd, frameX, frameY);

					RECT* rect = reinterpret_cast<RECT*>(lParam);
					rect->left += frameX;
					rect->top += frameY;
					rect->right -= frameX;
					rect->bottom -= frameY;
				}

				return 0;
			}
			case WM_NCHITTEST:
			{
				POINT point{ static_cast<int>(static_cast<short>(LOWORD(lParam))), static_cast<int>(static_cast<short>(HIWORD(lParam))) };
				ScreenToClient(hwnd, &point);

				if (!IsZoomed(hwnd))
				{
					RECT client{};
					GetClientRect(hwnd, &client);

					int frameX, frameY;
					GetNativeFrameSize(hwnd, frameX, frameY);

					bool left = point.x < frameX;
					bool right = point.x >= client.right - frameX;
					bool top = point.y < frameY;
					bool bottom = point.y >= client.bottom - frameY;

					if (top && left) return HTTOPLEFT;
					if (top && right) return HTTOPRIGHT;
					if (bottom && left) return HTBOTTOMLEFT;
					if (bottom && right) return HTBOTTOMRIGHT;
					if (left) return HTLEFT;
					if (right) return HTRIGHT;
					if (top) return HTTOP;
					if (bottom) return HTBOTTOM;
				}

				if (state->windowManager->GetHitTestTitleBar(point.x, point.y))
				{
					return HTCAPTION;
				}

				return HTCLIENT;
			}
			case WM_NCDESTROY:
			{
				WNDPROC prevWindowProc = state->prevWindowProc;
				SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
				delete state;
				return CallWindowProcW(prevWindowProc, hwnd, msg, wParam, lParam);
			}
		}

		return CallWindowProcW(state->prevWindowProc, hwnd, msg, wParam, lParam);
	}

	void WindowManagerGLFW::EnableCustomTitleBarNative()
	{
		HWND hwnd = glfwGetWin32Window(_window);
		if (!hwnd || GetWindowLongPtrW(hwnd, GWLP_USERDATA) != 0)
		{
			return;
		}

		auto state = new NativeTitleBarState{ this, reinterpret_cast<WNDPROC>(GetWindowLongPtrW(hwnd, GWLP_WNDPROC)) };
		SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(state));
		SetWindowLongPtrW(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(TitleBarWindowProc));

		MARGINS margins{ 0, 0, 1, 0 };
		DwmExtendFrameIntoClientArea(hwnd, &margins);

		SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

		LOGGER(info) << "[window] custom title bar enabled";
	}
}

#endif
