#pragma once

#include "seri/util/Util.h"
#include "seri/scene/SceneManager.h"

namespace seri::system
{
	class TransformSystem
	{
	public:
		static void Update();

	private:
		static void UpdateRecursive(const seri::scene::SceneTreeNode& node, const glm::mat4& parentWorld, bool parentActive);
	};
}
