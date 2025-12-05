#include "Seripch.h"

#include "seri/random/Random.h"

namespace seri
{
	static std::mt19937 sRandom_f{ std::random_device{}() };
	static std::uniform_real_distribution<float> sDist_f(0.0f, 1.0f);

	static std::mt19937_64 sRandom_uint64{ std::random_device{}() };
	static std::uniform_int_distribution<uint64_t> sDist_uint64;

	float Random::Range(float min, float max)
	{
		float r = sDist_f(sRandom_f);
		return Util::Map(r, 0.0f, 1.0f, min, max);
	}

	uint64_t Random::UUID()
	{
		uint64_t uuid = sDist_uint64(sRandom_uint64);
		if (uuid == 0)
		{
			uuid = 1;
		}
		return uuid;
	}

}
