#pragma once

#include "seri/platform/Platform.h"
#include "seri/logging/Logger.h"

#include <windows.h>
#include <timeapi.h>
#include <dwmapi.h>

#include <string>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "dwmapi.lib")

namespace seri::platform
{
	struct NativeTitleBarState
	{
		std::function<bool(int x, int y)> hitTest;
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

				if (state->hitTest && state->hitTest(point.x, point.y))
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

	std::filesystem::path GetExecutablePath()
	{
		std::wstring buffer(32768, L'\0');
		DWORD length = GetModuleFileNameW(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));
		buffer.resize(length);
		return std::filesystem::path{ buffer };
	}

	void BeginHighResolutionTimer()
	{
		timeBeginPeriod(1);
	}

	void EndHighResolutionTimer()
	{
		timeEndPeriod(1);
	}

	bool EnableConsoleColor()
	{
		HANDLE handle = GetStdHandle(STD_ERROR_HANDLE);
		if (handle == nullptr || handle == INVALID_HANDLE_VALUE)
		{
			return false;
		}

		DWORD mode = 0;
		if (!GetConsoleMode(handle, &mode))
		{
			return false;
		}

		return SetConsoleMode(handle, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING) != 0;
	}

	void EnableCustomTitleBar(void* nativeWindow, const std::function<bool(int x, int y)>& hitTest)
	{
		HWND hwnd = static_cast<HWND>(nativeWindow);
		if (!hwnd)
		{
			return;
		}

		if (auto state = reinterpret_cast<NativeTitleBarState*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA)))
		{
			state->hitTest = hitTest;
			return;
		}

		auto state = new NativeTitleBarState{ hitTest, reinterpret_cast<WNDPROC>(GetWindowLongPtrW(hwnd, GWLP_WNDPROC)) };
		SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(state));
		SetWindowLongPtrW(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(TitleBarWindowProc));

		MARGINS margins{ 0, 0, 1, 0 };
		DwmExtendFrameIntoClientArea(hwnd, &margins);

		SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

		LIB_LOGGER(info, platform) << "custom title bar enabled";
	}
}
