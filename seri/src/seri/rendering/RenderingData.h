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
		glm::vec4 params;  // x=innerCos, y=outerCos, z=constant, w=linear
		glm::vec4 params2; // x=quadratic, y/z/w=unused
	};

	struct UniformBufferPointLight
	{
		glm::vec4 position;
		glm::vec4 color;
		glm::vec4 params; // x=constant, y=linear, z=quadratic, w=unused
	};

	struct UniformBufferLight
	{
		UniformBufferDirectionalLight dirLight;
		glm::ivec4 dirLightExist{ 0 };

		UniformBufferSpotLight spotLights[literals::kMaxSpotLightCount];
		glm::ivec4 spotLightCount{ 0 };

		UniformBufferPointLight pointLights[literals::kMaxPointLightCount];
		glm::ivec4 pointLightCount{ 0 };
	};
}
