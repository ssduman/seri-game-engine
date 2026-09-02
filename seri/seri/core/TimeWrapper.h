#pragma once

#include "seri/core/Singleton.h"

#include "seri/util/Util.h"

#include <chrono>

namespace seri
{
	class TimeWrapper : public seri::Singleton<TimeWrapper>
	{
	public:
		static void Init()
		{
			GetInstance();
		}

		static void UpdateTime(double time)
		{
			GetInstance()._frameCount += 1;
			GetInstance()._time = static_cast<float>(time);
			GetInstance()._deltaTime = GetInstance()._time - GetInstance()._lastFrame;
			GetInstance()._lastFrame = GetInstance()._time;
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

		static int GetFPS()
		{
			return Util::RountToInt(1.0f / TimeWrapper::GetDeltaTime());
		}

	protected:
		friend struct seri::Singleton<TimeWrapper>;

		TimeWrapper() = default;
		~TimeWrapper() = default;

	private:
		float _time{ 0.0f };
		float _deltaTime{ 0.016f };
		double _lastFrame{ 0.0 };

		float _scale{ 1.0f };

		unsigned int _frameCount{ 0 };

	};
}
