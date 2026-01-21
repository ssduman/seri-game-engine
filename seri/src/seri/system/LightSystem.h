#pragma once

#include "seri/util/Util.h"
#include "seri/scene/SceneManager.h"
#include "seri/component/Components.h"
#include "seri/rendering/RenderingData.h"
#include "seri/rendering/RenderingManager.h"

namespace seri::system
{
	class LightSystem
	{
	public:
		static void Update();

	private:
		static glm::vec3 GetForward(const glm::vec3& eulerDeg);

	};
}
