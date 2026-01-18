#pragma once

#include "seri/util/Util.h"
#include "seri/util/YAMLUtil.h"

#include <entt/entt.hpp>
#include <yaml-cpp/yaml.h>

namespace seri::component
{
	struct IDComponent
	{
		static constexpr std::string_view compName = "IDComponent";

		uint64_t id{ 0 };
		uint64_t parentId{ 0 };
		std::string name{ "" };

		static IDComponent Deserialize(const YAML::Node& node);
		static YAML::Node Serialize(IDComponent component);
	};

	struct TransformComponent
	{
		static constexpr std::string_view compName = "TransformComponent";

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
		static constexpr std::string_view compName = "SceneComponent";

		std::string version{ "0.0" };
		bool isActive{ true };

		static SceneComponent Deserialize(const YAML::Node& node);
		static YAML::Node Serialize(SceneComponent component);
	};

	struct MeshComponent
	{
		static constexpr std::string_view compName = "MeshComponent";

		uint64_t meshAssetId{ 0 };

		static MeshComponent Deserialize(const YAML::Node& node);
		static YAML::Node Serialize(MeshComponent component);
	};

	struct MeshRendererComponent
	{
		static constexpr std::string_view compName = "MeshRendererComponent";

		uint64_t materialAssetId{ 0 };
		bool castShadow = true;

		static MeshRendererComponent Deserialize(const YAML::Node& node);
		static YAML::Node Serialize(MeshRendererComponent component);
	};

	struct DirectionalLightComponent
	{
		static constexpr std::string_view compName = "DirectionalLightComponent";

		float intensity = 1.0f;
		glm::vec3 color{ 1.0f, 1.0f, 1.0f };

		static DirectionalLightComponent Deserialize(const YAML::Node& node);
		static YAML::Node Serialize(DirectionalLightComponent component);
	};

	struct SpotLightComponent
	{
		static constexpr std::string_view compName = "SpotLightComponent";

		float intensity = 10.0f;
		float innerAngle = 20.0f;
		float outerAngle = 30.0f;
		glm::vec3 color{ 1.0f, 1.0f, 1.0f };

		static SpotLightComponent Deserialize(const YAML::Node& node);
		static YAML::Node Serialize(SpotLightComponent component);
	};

	struct PointLightComponent
	{
		static constexpr std::string_view compName = "PointLightComponent";

		float range = 10.0f;
		float intensity = 10.0f;
		glm::vec3 color{ 1.0f, 1.0f, 1.0f };

		static PointLightComponent Deserialize(const YAML::Node& node);
		static YAML::Node Serialize(PointLightComponent component);
	};

}
