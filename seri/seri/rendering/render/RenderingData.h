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

	struct UniformBufferMaterial
	{
		glm::vec4 baseColor{ 1.0f, 1.0f, 1.0f, 1.0f };

		glm::ivec4 uses{ 1, 1, 1,0 }; // x=diff, y=normal, z=arm
		glm::vec4 modifiers{ 0.0f, 0.0f, 0.0f, 0.0f }; // x=ao, y=roughness, z=metallic, w=unused
	};

	struct UniformBufferShadow
	{
		glm::vec4 modifiers{ 0.0005f, 0.002f, 1.0f, 0.0f }; // x=shadow bias, y=normal bias, z=shadow strength, w=unused

		glm::mat4 dirLightViewProj;

		glm::mat4 spotLightViewProj[literals::kMaxSpotLightShadowCount];
		glm::ivec4 spotLightShadowCount{ 0 };
	};
}
