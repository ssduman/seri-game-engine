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
		static void Update()
		{
			const auto& registry = scene::SceneManager::GetRegistry();

			UniformBufferLight outUBO{};

			auto dirView = registry.view<component::TransformComponent, component::DirectionalLightComponent>();
			for (auto entity : dirView)
			{
				if (outUBO.dirLightExists >= literals::kMaxDirLightCount)
				{
					break;
				}

				auto& transform = dirView.get<component::TransformComponent>(entity);
				auto& light = dirView.get<component::DirectionalLightComponent>(entity);

				outUBO.dirLightExists = 1;

				auto& gpuLight = outUBO.dirLight;
				gpuLight.direction = glm::vec4(GetForward(transform.rotation), 0.0f);
				gpuLight.color = glm::vec4(light.color * light.intensity, 1.0f);
			}

			auto spotView = registry.view<component::TransformComponent, component::SpotLightComponent>();
			for (auto entity : spotView)
			{
				if (outUBO.spotLightCount >= literals::kMaxSpotLightCount)
				{
					break;
				}

				auto& transform = spotView.get<component::TransformComponent>(entity);
				auto& light = spotView.get<component::SpotLightComponent>(entity);

				auto& gpuLight = outUBO.spotLights[outUBO.spotLightCount++];
				gpuLight.position = glm::vec4(transform.position, 1.0f);
				gpuLight.direction = glm::vec4(GetForward(transform.rotation), 0.0f);
				gpuLight.color = glm::vec4(light.color * light.intensity, 1.0f);
				gpuLight.params = glm::vec4(
					std::cos(glm::radians(light.innerAngle)),
					std::cos(glm::radians(light.outerAngle)),
					light.constant,
					light.linear
				);
				gpuLight.params2 = glm::vec4(light.quadratic, 0.0f, 0.0f, 0.0f);
			}

			auto pointView = registry.view<component::TransformComponent, component::PointLightComponent>();
			for (auto entity : pointView)
			{
				if (outUBO.pointLightCount >= literals::kMaxPointLightCount)
				{
					break;
				}

				auto& transform = pointView.get<component::TransformComponent>(entity);
				auto& light = pointView.get<component::PointLightComponent>(entity);

				auto& gpuLight = outUBO.pointLights[outUBO.pointLightCount++];
				gpuLight.position = glm::vec4(transform.position, 1.0f);
				gpuLight.color = glm::vec4(light.color * light.intensity, 1.0f);
				gpuLight.attenuation = glm::vec4(light.constant, light.linear, light.quadratic, 0.0f);
			}

			RenderingManager::GetLightUBO()->SetData(&outUBO, sizeof(UniformBufferLight));
		}

	private:
		static glm::vec3 GetForward(const glm::vec3& eulerDeg)
		{
			float pitch = glm::radians(eulerDeg.x);
			float yaw = glm::radians(eulerDeg.y);

			//glm::vec3 forward;
			//forward.x = cos(yaw) * cos(pitch);
			//forward.y = sin(pitch);
			//forward.z = sin(yaw) * cos(pitch);

			glm::vec3 forward;
			forward.x = cos(pitch) * sin(yaw);
			forward.y = sin(pitch);
			forward.z = -cos(pitch) * cos(yaw);

			return glm::normalize(forward);
		}

	};
}
