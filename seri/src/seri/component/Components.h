#pragma once

#include "seri/util/Util.h"

#include <yaml-cpp/yaml.h>

namespace seri
{
	namespace component
	{
		struct Util
		{
			static glm::vec3 Vec3FromYAML(const YAML::Node& node)
			{
				return { node[0].as<float>(), node[1].as<float>(), node[2].as<float>() };
			}

			static YAML::Node Vec3ToYAML(const glm::vec3& vec)
			{
				YAML::Node node;
				node.push_back(vec.x);
				node.push_back(vec.y);
				node.push_back(vec.z);
				return node;
			}
		};
	}

	struct IDComponent
	{
		uint32_t id{ 0 };
		uint32_t parentId{ 0 };
		std::string name{ "" };

		static IDComponent Deserialize(const YAML::Node& node)
		{
			IDComponent component{};
			component.id = node["ID"].as<uint32_t>();
			component.parentId = node["ParentID"].as<uint32_t>();
			component.name = node["Name"].as<std::string>();
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
			component.position = component::Util::Vec3FromYAML(node["Position"]);
			component.rotation = component::Util::Vec3FromYAML(node["Rotation"]);
			component.scale = component::Util::Vec3FromYAML(node["Scale"]);
			return component;
		}

		static YAML::Node Serialize(TransformComponent component)
		{
			YAML::Node node;
			node["Position"] = component::Util::Vec3ToYAML(component.position);
			node["Rotation"] = component::Util::Vec3ToYAML(component.rotation);
			node["Scale"] = component::Util::Vec3ToYAML(component.scale);
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
