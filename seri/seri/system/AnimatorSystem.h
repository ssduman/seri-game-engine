#pragma once

#include <memory>

namespace seri
{
	class Model;
	struct Animation;
}

namespace seri::component
{
	struct AnimatorComponent;
}

namespace seri::system
{
	class AnimatorSystem
	{
	public:
		static void Update();

		static const seri::Animation* GetClip(const seri::component::AnimatorComponent* animator, const std::shared_ptr<seri::Model>& model);
	};
}
