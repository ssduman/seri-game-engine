#pragma once

#include "seri/core/Core.h"
#include "seri/core/Singleton.h"

#include "seri/util/Util.h"
#include "seri/window/WindowManager.h"

#include <chrono>

namespace seri
{
	class Application : public seri::Singleton<Application>
	{
	public:
		static void Init()
		{
			GetInstance();
		}

		static void SetVSyncCount(int count)
		{
			GetInstance()._vSyncCount = count;
			WindowManager::SetVSyncCount(count);
		}

		static void SetTargetFrameRate(int frameRate)
		{
			GetInstance()._targetFrameRate = static_cast<double>(frameRate);
			GetInstance()._targetMs = 1000.0 / GetInstance()._targetFrameRate;
		}

		static void SetRunInBackground(bool runInBackground)
		{
			GetInstance()._runInBackground = runInBackground;
		}

		static void SetFrameBegin()
		{
			GetInstance()._frameBegTime = std::chrono::steady_clock::now();
		}

		static void SetFrameEnd()
		{
			GetInstance()._frameEndTime = std::chrono::steady_clock::now();
		}

		static double GetWaitTime()
		{
			auto timetaken = GetInstance()._frameEndTime - GetInstance()._frameBegTime;
			auto timetakenMs = std::chrono::duration<double, std::milli>(timetaken).count();
			return GetInstance()._targetMs - timetakenMs;
		}

		static void WaitForTargetFrameRate();

	protected:
		friend struct seri::Singleton<Application>;

		Application();
		~Application();

	private:
		int _vSyncCount{ 0 };
		double _targetFrameRate{ 60.0 };
		bool _runInBackground{ true };

		double _targetMs = 1000.0 / _targetFrameRate;

		std::chrono::steady_clock::time_point _frameBegTime;
		std::chrono::steady_clock::time_point _frameEndTime;

	};
}
