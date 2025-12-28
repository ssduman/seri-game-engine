#pragma once

#include "seri/util/Util.h"
#include "seri/util/YAMLUtil.h"

#include <entt/entt.hpp>
#include <yaml-cpp/yaml.h>

namespace seri::component
{
	struct IDComponent
	{
		uint64_t id{ 0 };
		uint64_t parentId{ 0 };
		std::string name{ "" };

		static IDComponent Deserialize(const YAML::Node& node);

		static YAML::Node Serialize(IDComponent component);
	};

	struct TransformComponent
	{
		glm::vec3 position{ 0.0f, 0.0f, 0.0f };
		glm::vec3 rotation{ 0.0f, 0.0f, 0.0f };
		glm::vec3 scale{ 1.0f, 1.0f, 1.0f };

		glm::mat4 localMatrix{ 1.0f };
		glm::mat4 worldMatrix{ 1.0f };

		static TransformComponent Deserialize(const YAML::Node& node);

		static YAML::Node Serialize(TransformComponent component);
	};

	struct SceneComponent
	{
		std::string version{ "0.0" };
		bool isActive{ true };

		static SceneComponent Deserialize(const YAML::Node& node);

		static YAML::Node Serialize(SceneComponent component);
	};

	struct MeshComponent
	{
		uint64_t meshAssetId{ 0 };

		static MeshComponent Deserialize(const YAML::Node& node);

		static YAML::Node Serialize(MeshComponent component);
	};

	struct MeshRendererComponent
	{
		uint64_t materialAssetId{ 0 };
		bool castShadow = true;

		static MeshRendererComponent Deserialize(const YAML::Node& node);

		static YAML::Node Serialize(MeshRendererComponent component);
	};
}
