#pragma once

#include "seri/util/Util.h"
#include "seri/scene/SceneManager.h"
#include "seri/camera/CameraBase.h"
#include "seri/component/Components.h"

namespace seri::system
{
	class CameraSystem
	{
	public:
		static void Update();

		static void DrawFrustumGizmo(entt::entity entity);

	private:
		static entt::entity FindMainCameraEntity();

		static CameraProperties BuildCameraProperties(const component::TransformComponent& transform, const component::CameraComponent& camera);

	};
}
