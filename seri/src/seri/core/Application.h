#pragma once

#pragma once

#include "seri/util/Util.h"
#include "seri/window/WindowManagerFactory.h"

#include <chrono>

namespace seri
{
	class Application
	{
	public:
		Application(Application const&) = delete;

		void operator=(Application const&) = delete;

		static Application& GetInstance()
		{
			static Application instance;
			return instance;
		}

		static void Init()
		{
			GetInstance();
		}

		static void SetVSyncCount(int count)
		{
			GetInstance()._vSyncCount = count;
			WindowManagerFactory::instance()->SetVSyncCount(count);
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
			GetInstance()._frameBegTime = std::chrono::high_resolution_clock::now();
		}
		
		static void SetFrameEnd()
		{
			GetInstance()._frameEndTime = std::chrono::high_resolution_clock::now();
		}

		static double GetWaitTime()
		{
			auto timetaken = GetInstance()._frameEndTime - GetInstance()._frameBegTime;
			auto timetakenMs = std::chrono::duration<double, std::milli>(timetaken).count();
			return GetInstance()._targetMs - timetakenMs;
		}

	private:
		Application() = default;
		~Application() = default;

		int _vSyncCount{ 0 };
		double _targetFrameRate{ 60.0 };
		bool _runInBackground{ true };

		double _targetMs = 1000.0 / _targetFrameRate;

		std::chrono::high_resolution_clock::time_point _frameBegTime;
		std::chrono::high_resolution_clock::time_point _frameEndTime;

	};
}
