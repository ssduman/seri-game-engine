#pragma once

#include "seri/logging/Logger.h"

#include <yaml-cpp/yaml.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <memory>
#include <string>
#include <unordered_map>

namespace seri
{
	struct YAMLUtil
	{
		template<typename T>
		static T GetType(const YAML::Node& node)
		{
			return node.as<T>();
		}

		static glm::vec2 Vec2FromYAML(const YAML::Node& node)
		{
			return { node[0].as<float>(), node[1].as<float>() };
		}

		static glm::vec3 Vec3FromYAML(const YAML::Node& node)
		{
			return { node[0].as<float>(), node[1].as<float>(), node[2].as<float>() };
		}

		static glm::vec4 Vec4FromYAML(const YAML::Node& node)
		{
			return { node[0].as<float>(), node[1].as<float>(), node[2].as<float>(), node[3].as<float>() };
		}

		static glm::mat4 Mat4FromYAML(const YAML::Node& node)
		{
			glm::mat4 mat{};

			if (!node.IsSequence() || node.size() != 4)
			{
				return mat;
			}

			for (int row = 0; row < 4; row++)
			{
				const auto& rowNode = node[row];
				if (!rowNode.IsSequence() || rowNode.size() != 4)
				{
					continue;
				}

				for (int col = 0; col < 4; col++)
				{
					mat[row][col] = rowNode[col].as<float>();
				}
			}

			return mat;
		}

		static YAML::Node Vec2ToYAML(const glm::vec2& vec)
		{
			YAML::Node node;
			node.push_back(vec.x);
			node.push_back(vec.y);
			node.SetStyle(YAML::EmitterStyle::Flow);
			return node;
		}

		static YAML::Node Vec3ToYAML(const glm::vec3& vec)
		{
			YAML::Node node;
			node.push_back(vec.x);
			node.push_back(vec.y);
			node.push_back(vec.z);
			node.SetStyle(YAML::EmitterStyle::Flow);
			return node;
		}

		static YAML::Node Vec4ToYAML(const glm::vec4& vec)
		{
			YAML::Node node;
			node.push_back(vec.x);
			node.push_back(vec.y);
			node.push_back(vec.z);
			node.push_back(vec.w);
			node.SetStyle(YAML::EmitterStyle::Flow);
			return node;
		}

		static YAML::Node Mat4ToYAML(const glm::mat4& mat)
		{
			YAML::Node node;

			for (int row = 0; row < 4; row++)
			{
				YAML::Node rowNode;
				for (int col = 0; col < 4; col++)
				{
					rowNode.push_back(mat[row][col]);
					rowNode.SetStyle(YAML::EmitterStyle::Flow);
				}

				node.push_back(rowNode);
			}

			return node;
		}

		static std::string DeepCopyYAMLString(const YAML::Node& n)
		{
			auto s = n.as<std::string>();
			return std::string(s.data(), s.size());
		}

	};
}
