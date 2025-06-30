#pragma once

#include "seri/util/Util.h"

#include <chrono>

namespace seri
{
	class TimeWrapper
	{
	public:
		TimeWrapper(TimeWrapper const&) = delete;

		void operator=(TimeWrapper const&) = delete;

		static TimeWrapper& GetInstance()
		{
			static TimeWrapper instance;
			return instance;
		}

		static void Init()
		{
			GetInstance();
		}

		static void RegisterTime(float currTime, float deltaTime)
		{
			GetInstance()._frameCount += 1;
			GetInstance()._time = currTime;
			GetInstance()._deltaTime = deltaTime;
		}

		static unsigned int GetFrameCount()
		{
			return GetInstance()._frameCount;
		}

		static float GetTime()
		{
			return GetInstance()._time;
		}

		static float GetDeltaTime()
		{
			return GetInstance()._deltaTime;
		}

		inline static auto GetPresiceTime()
		{
			return std::chrono::high_resolution_clock::now().time_since_epoch().count();
		}

	private:
		TimeWrapper() = default;
		~TimeWrapper() = default;

		float _time{ 0.0f };
		float _deltaTime{ 0.016f };

		float _scale{ 1.0f };

		unsigned int _frameCount{ 0 };

	};
}
