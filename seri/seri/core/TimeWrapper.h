#pragma once

#include "seri/core/Singleton.h"

#include "seri/util/Util.h"

#include <chrono>
#include <algorithm>

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
			auto& instance = GetInstance();

			instance._frameCount += 1;
			instance._time = static_cast<float>(time);
			instance._deltaTime = instance._time - instance._lastFrame;
			instance._lastFrame = instance._time;

			instance._fixedDeltaTimeAccumulator = std::min(instance._fixedDeltaTimeAccumulator + instance._deltaTime, instance._fixedDeltaTime * instance._maxFixedSteps);
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

		static float GetFixedDeltaTime()
		{
			return GetInstance()._fixedDeltaTime;
		}

		static void SetFixedDeltaTime(float fixedDeltaTime)
		{
			GetInstance()._fixedDeltaTime = std::max(fixedDeltaTime, kMinFixedDeltaTime);
		}

		inline static auto GetPresiceTime()
		{
			return std::chrono::high_resolution_clock::now().time_since_epoch().count();
		}

		static int GetFPS()
		{
			return Util::RountToInt(1.0f / TimeWrapper::GetDeltaTime());
		}

		static bool TickFixedStep()
		{
			auto& instance = GetInstance();

			if (instance._fixedDeltaTimeAccumulator >= instance._fixedDeltaTime)
			{
				instance._fixedDeltaTimeAccumulator -= instance._fixedDeltaTime;

				return true;
			}

			return false;
		}

		static void ResetFixedStep()
		{
			GetInstance()._fixedDeltaTimeAccumulator = 0.0f;
		}

	protected:
		friend struct seri::Singleton<TimeWrapper>;

		TimeWrapper() = default;
		~TimeWrapper() = default;

	private:
		static inline const float kMinFixedDeltaTime{ 0.001f };

		float _time{ 0.0f };
		float _deltaTime{ 0.016f };
		float _fixedDeltaTime{ 1.0f / 60.0f };
		float _fixedDeltaTimeAccumulator{ 0.0f };
		int _maxFixedSteps{ 8 };
		double _lastFrame{ 0.0 };

		float _scale{ 1.0f };

		unsigned int _frameCount{ 0 };

	};
}
