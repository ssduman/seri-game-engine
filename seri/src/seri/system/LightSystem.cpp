#include "Seripch.h"
#include "LightSystem.h"

namespace seri::system
{
	void LightSystem::Update()
	{
		const auto& registry = scene::SceneManager::GetRegistry();

		UniformBufferLight outUBO{};

		auto dirView = registry.view<component::TransformComponent, component::DirectionalLightComponent>();
		for (auto entity : dirView)
		{
			if (outUBO.dirLightExist.x >= literals::kMaxDirLightCount)
			{
				break;
			}

			auto& transform = dirView.get<component::TransformComponent>(entity);
			auto& light = dirView.get<component::DirectionalLightComponent>(entity);

			outUBO.dirLightExist.x = 1;

			UniformBufferDirectionalLight& dirLightUBO = outUBO.dirLight;
			dirLightUBO.direction = glm::vec4(GetForward(transform.rotation), 0.0f);
			dirLightUBO.color = glm::vec4(light.color * light.intensity, 1.0f);
		}

		auto spotView = registry.view<component::TransformComponent, component::SpotLightComponent>();
		for (auto entity : spotView)
		{
			if (outUBO.spotLightCount.x >= literals::kMaxSpotLightCount)
			{
				break;
			}

			auto& transform = spotView.get<component::TransformComponent>(entity);
			auto& light = spotView.get<component::SpotLightComponent>(entity);

			UniformBufferSpotLight& spotLight = outUBO.spotLights[outUBO.spotLightCount.x++];
			spotLight.position = glm::vec4(transform.position, 0.0f);
			spotLight.direction = glm::vec4(GetForward(transform.rotation), 0.0f);
			spotLight.color = glm::vec4(light.color * light.intensity, 1.0f);
			spotLight.params = glm::vec4(
				std::cos(glm::radians(light.innerAngle)),
				std::cos(glm::radians(light.outerAngle)),
				light.constant,
				light.linear
			);
			spotLight.params2 = glm::vec4(light.quadratic, 0.0f, 0.0f, 0.0f);
		}

		auto pointView = registry.view<component::TransformComponent, component::PointLightComponent>();
		for (auto entity : pointView)
		{
			if (outUBO.pointLightCount.x >= literals::kMaxPointLightCount)
			{
				break;
			}

			auto& transform = pointView.get<component::TransformComponent>(entity);
			auto& light = pointView.get<component::PointLightComponent>(entity);

			UniformBufferPointLight& pointLightUBO = outUBO.pointLights[outUBO.pointLightCount.x++];
			pointLightUBO.position = glm::vec4(transform.position, 1.0f);
			pointLightUBO.color = glm::vec4(light.color * light.intensity, 1.0f);
			pointLightUBO.params = glm::vec4(light.constant, light.linear, light.quadratic, 0.0f);
		}

		RenderingManager::GetLightUBO()->SetData(&outUBO, sizeof(UniformBufferLight));

		if (outUBO.dirLightExist.x == 1)
		{
			glm::vec3 lightDir = glm::normalize(glm::vec3(outUBO.dirLight.direction));

			glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
			if (glm::abs(glm::dot(lightDir, up)) > 0.999f)
			{
				up = glm::vec3(1.0f, 0.0f, 0.0f);
			}

			const float orthoSize = 20.0f;
			const float nearPlane = 0.1f;
			const float farPlane = 200.0f;
			const float lightDist = 100.0f;
			glm::vec3 lightPos = -lightDir * lightDist;

			glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), up);
			glm::mat4 lightProj = glm::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, nearPlane, farPlane);
			glm::mat4 lightViewProj = lightProj * lightView;

			UniformBufferShadow shadowUBOData{};
			shadowUBOData.dirLightViewProj = lightViewProj;

			RenderingManager::SetShadowLightViewProj(lightViewProj);
			RenderingManager::GetShadowUBO()->SetData(&shadowUBOData, sizeof(UniformBufferShadow));
		}
	}

	glm::vec3 LightSystem::GetForward(const glm::vec3& eulerDeg)
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
}
