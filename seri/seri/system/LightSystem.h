#pragma once

#include "seri/util/Util.h"
#include "seri/scene/SceneManager.h"
#include "seri/component/Components.h"
#include "seri/rendering/render/RenderingData.h"
#include "seri/rendering/render/RenderingManager.h"

namespace seri::system
{
	class LightSystem
	{
	public:
		static void Update();

		static void DrawDirectionalLightGizmo(entt::entity entity);

		static void DrawSpotLightGizmo(entt::entity entity);

		static void DrawPointLightGizmo(entt::entity entity);

	private:
		static glm::vec3 GetForward(const glm::vec3& eulerDeg);

		static glm::mat3 GetBasis(const glm::mat4& worldMatrix);

	};
}
