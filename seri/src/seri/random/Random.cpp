#include "Seripch.h"

#include "seri/random/Random.h"

namespace seri
{
	static std::random_device _rd;
	static std::mt19937 _gen_f(_rd());
	static std::uniform_real_distribution<float> _distrib_f(0.0f, 1.0f);

	float Random::Range(float min, float max)
	{
		float r = _distrib_f(_gen_f);

		return Util::Map(r, 0.0f, 1.0f, min, max);
	}

}
