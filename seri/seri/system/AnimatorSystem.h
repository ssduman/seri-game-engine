#pragma once

#include <memory>
#include <cstdint>

namespace seri
{
	class Model;
	struct Animation;
}

namespace seri::system
{
	class AnimatorSystem
	{
	public:
		static void Update();

		static const seri::Animation* GetClip(uint64_t clipAssetId, int clipIndex, const std::shared_ptr<seri::Model>& model);

	private:
		static float WrapTime(float time, double duration, bool loop);
	};
}
