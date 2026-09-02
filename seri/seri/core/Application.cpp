#include "Seripch.h"

#include "seri/core/Application.h"

#include <thread>

#ifdef SERI_WINDOWS

#include <windows.h>
#include <timeapi.h>

#pragma comment(lib, "winmm.lib")

#endif

namespace seri
{
	Application::Application()
	{
#ifdef SERI_WINDOWS
		timeBeginPeriod(1);
#endif
	}

	Application::~Application()
	{
#ifdef SERI_WINDOWS
		timeEndPeriod(1);
#endif
	}

	void Application::WaitForTargetFrameRate()
	{
		auto& instance = GetInstance();

		if (instance._vSyncCount == 0)
		{
			return;
		}

		auto targetDuration = std::chrono::duration_cast<std::chrono::steady_clock::duration>(
			std::chrono::duration<double, std::milli>(instance._targetMs)
		);

		auto deadline = instance._frameBegTime + targetDuration;
		auto sleepUntil = deadline - std::chrono::milliseconds(2);

		if (std::chrono::steady_clock::now() < sleepUntil)
		{
			std::this_thread::sleep_until(sleepUntil);
		}

		while (std::chrono::steady_clock::now() < deadline)
		{
			std::this_thread::yield();
		}
	}
}
