#pragma once

#include "seri/util/Util.h"

namespace seri
{
	struct IDComponent
	{
		uint32_t id{ 0 };
		std::string name{ "Entity" };
	};

	struct TransformComponent
	{
		glm::vec3 position{ 0.0f, 0.0f, 0.0f };
		glm::vec3 rotation{ 0.0f, 0.0f, 0.0f };
		glm::vec3 scale{ 1.0f, 1.0f, 1.0f };
	};
}
