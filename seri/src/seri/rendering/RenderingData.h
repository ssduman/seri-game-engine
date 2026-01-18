#pragma once

#include "seri/util/Util.h"

namespace seri
{
	struct UniformBufferCamera
	{
		glm::mat4 view;
		glm::mat4 proj;
		glm::mat4 viewProj;

		glm::vec4 cameraPos;
		glm::vec4 time; // x=time, y=delta
	};

	struct UniformBufferDirectionalLight
	{
		glm::vec4 direction;
		glm::vec4 color;
	};

	struct UniformBufferSpotLight
	{
		glm::vec4 position;
		glm::vec4 direction;
		glm::vec4 color;
		glm::vec4 params; // x=innerCos, y=outerCos
	};

	struct UniformBufferPointLight
	{
		glm::vec4 position;
		glm::vec4 color;
	};

	struct UniformBufferLight
	{
		UniformBufferDirectionalLight dirLight;
		alignas(16) int dirLightExists;
		alignas(16) glm::vec3 pad0;

		UniformBufferSpotLight spotLights[literals::kMaxSpotLightCount];
		alignas(16) int spotLightCount;
		alignas(16) glm::vec3 pad1;

		UniformBufferPointLight pointLights[literals::kMaxPointLightCount];
		alignas(16) int pointLightCount;
		alignas(16) glm::vec3 pad2;
	};
}
