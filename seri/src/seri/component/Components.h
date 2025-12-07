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

		static IDComponent Deserialize(const YAML::Node& node)
		{
			IDComponent component{};
			component.id = node["ID"].as<uint64_t>();
			component.parentId = node["ParentID"].as<uint64_t>();
			component.name = YAMLUtil::DeepCopyYAMLString(node["Name"]);
			return component;
		}

		static YAML::Node Serialize(IDComponent component)
		{
			YAML::Node node;
			node["ID"] = component.id;
			node["ParentID"] = component.parentId;
			node["Name"] = component.name;
			return node;
		}
	};

	struct TransformComponent
	{
		glm::vec3 position{ 0.0f, 0.0f, 0.0f };
		glm::vec3 rotation{ 0.0f, 0.0f, 0.0f };
		glm::vec3 scale{ 1.0f, 1.0f, 1.0f };

		static TransformComponent Deserialize(const YAML::Node& node)
		{
			TransformComponent component{};
			component.position = YAMLUtil::Vec3FromYAML(node["Position"]);
			component.rotation = YAMLUtil::Vec3FromYAML(node["Rotation"]);
			component.scale = YAMLUtil::Vec3FromYAML(node["Scale"]);
			return component;
		}

		static YAML::Node Serialize(TransformComponent component)
		{
			YAML::Node node;
			node["Position"] = YAMLUtil::Vec3ToYAML(component.position);
			node["Rotation"] = YAMLUtil::Vec3ToYAML(component.rotation);
			node["Scale"] = YAMLUtil::Vec3ToYAML(component.scale);
			return node;
		}
	};

	struct SceneComponent
	{
		std::string version{ "0.0" };
		bool isActive{ true };

		static SceneComponent Deserialize(const YAML::Node& node)
		{
			SceneComponent component{};
			component.version = node["Version"].as<std::string>();
			component.isActive = node["IsActive"].as<bool>();
			return component;
		}

		static YAML::Node Serialize(SceneComponent component)
		{
			YAML::Node node;
			node["Version"] = component.version;
			node["IsActive"] = component.isActive;
			return node;
		}
	};
}
