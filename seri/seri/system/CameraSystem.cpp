#include "Seripch.h"
#include "CameraSystem.h"

#include "seri/graphic/Graphic.h"
#include "seri/camera/RuntimeCamera.h"
#include "seri/draw/DebugDraw.h"
#include "seri/rendering/render/RenderingManager.h"

namespace seri::system
{
	void CameraSystem::Update()
	{
		entt::entity entity = FindMainCameraEntity();
		if (entity == entt::null)
		{
			Graphic::SetRuntimeCamera(nullptr);
			return;
		}

		auto& registry = scene::SceneManager::GetRegistry();

		auto runtimeCamera = std::dynamic_pointer_cast<RuntimeCamera>(Graphic::GetRuntimeCamera());
		if (runtimeCamera == nullptr)
		{
			runtimeCamera = std::make_shared<RuntimeCamera>(CameraProperties{});
			Graphic::SetRuntimeCamera(runtimeCamera);
		}

		runtimeCamera->GetCameraProperties() = BuildCameraProperties(
			registry.get<component::TransformComponent>(entity),
			registry.get<component::CameraComponent>(entity)
		);

		runtimeCamera->Update();
	}

	void CameraSystem::DrawFrustumGizmo(entt::entity entity)
	{
		auto& registry = scene::SceneManager::GetRegistry();

		auto* transform = registry.try_get<component::TransformComponent>(entity);
		auto* camera = registry.try_get<component::CameraComponent>(entity);
		if (transform == nullptr || camera == nullptr)
		{
			return;
		}

		RuntimeCamera frustumCamera{ BuildCameraProperties(*transform, *camera) };
		frustumCamera.Update();

		debug::DebugDraw::DrawFrustum(frustumCamera.GetViewProjection(), glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });
	}

	entt::entity CameraSystem::FindMainCameraEntity()
	{
		auto& registry = scene::SceneManager::GetRegistry();

		entt::entity found = entt::null;

		auto view = registry.view<component::TransformComponent, component::CameraComponent>();
		for (auto entity : view)
		{
			if (view.get<component::CameraComponent>(entity).isMain)
			{
				return entity;
			}

			if (found == entt::null)
			{
				found = entity;
			}
		}

		return found;
	}

	CameraProperties CameraSystem::BuildCameraProperties(const component::TransformComponent& transform, const component::CameraComponent& camera)
	{
		float aspect = RenderingManager::GetGameRT()->GetAspectRatio();

		CameraProperties properties{};
		properties.isOrtho = camera.isOrtho;
		properties.fov = camera.fov;
		properties.nearPlane = camera.nearPlane;
		properties.farPlane = camera.farPlane;
		properties.aspect = aspect;
		properties.height = camera.orthoSize * 2.0f;
		properties.width = properties.height * aspect;

		glm::mat3 rotation{ transform.worldMatrix };
		rotation[0] = glm::normalize(rotation[0]);
		rotation[1] = glm::normalize(rotation[1]);
		rotation[2] = glm::normalize(rotation[2]);

		properties.position = glm::vec3{ transform.worldMatrix[3] };
		properties.rotation = glm::normalize(glm::quat_cast(rotation));

		return properties;
	}

}
