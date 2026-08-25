#pragma once

#include "util/Util.h"
#include "scene/SceneManager.h"
#include "component/Components.h"
#include "rendering/render/RenderingData.h"
#include "rendering/render/RenderingManager.h"

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
