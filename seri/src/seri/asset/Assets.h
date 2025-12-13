#pragma once

#include "seri/util/Util.h"
#include "seri/util/YAMLUtil.h"
#include "seri/graphic/Material.h"

namespace seri::asset
{
	struct IDInfo
	{
		uint64_t id{ 0 };
		std::string version{ "0.1" };

		static YAML::Node Serialize(const IDInfo& component)
		{
			YAML::Node node;
			node["ID"] = component.id;
			node["Version"] = component.version;
			return node;
		}

		static IDInfo Deserialize(const YAML::Node& node)
		{
			IDInfo component;
			component.id = node["ID"].as<uint64_t>();
			component.version = YAMLUtil::DeepCopyYAMLString(node["Version"]);
			return component;
		}
	};

	struct MaterialAsset
	{
		static YAML::Node Serialize(const std::shared_ptr<Material>& material)
		{
			YAML::Node node;

			YAML::Node intsNode;
			for (const auto& kv : material->GetInts())
			{
				intsNode[kv.first] = kv.second;
			}
			node["Ints"] = intsNode;

			YAML::Node floatsNode;
			for (const auto& kv : material->GetFloats())
			{
				floatsNode[kv.first] = kv.second;
			}
			node["Floats"] = floatsNode;

			YAML::Node float2sNode;
			for (const auto& kv : material->GetFloat2s())
			{
				float2sNode[kv.first] = YAMLUtil::Vec2ToYAML(kv.second);
			}
			node["Float2s"] = float2sNode;

			YAML::Node float3sNode;
			for (const auto& kv : material->GetFloat3s())
			{
				float3sNode[kv.first] = YAMLUtil::Vec3ToYAML(kv.second);
			}
			node["Float3s"] = float3sNode;

			YAML::Node float4sNode;
			for (const auto& kv : material->GetFloat4s())
			{
				float4sNode[kv.first] = YAMLUtil::Vec4ToYAML(kv.second);
			}
			node["Float4s"] = float4sNode;

			YAML::Node mat4sNode;
			for (const auto& kv : material->GetMats())
			{
				mat4sNode[kv.first] = YAMLUtil::Mat4ToYAML(kv.second);
			}
			node["Mat4s"] = mat4sNode;

			YAML::Node texturesNode;
			for (const auto& kv : material->GetTextures())
			{
				mat4sNode[kv.first] = kv.second->id;
			}
			node["Textures"] = mat4sNode;

			node["ShaderID"] = material->shader != nullptr ? material->shader->id : 0;

			return node;
		}

		static std::shared_ptr<Material> Deserialize(const YAML::Node& node)
		{
			std::shared_ptr<Material> material = std::make_shared<Material>();

			const YAML::Node& intsNode = node["Ints"];
			for (const auto& kv : intsNode)
			{
				std::string name = YAMLUtil::DeepCopyYAMLString(kv.first);
				int value = kv.second.as<int>();
				material->SetInt(name, value);
			}

			const YAML::Node& floatsNode = node["Floats"];
			for (const auto& kv : floatsNode)
			{
				std::string name = YAMLUtil::DeepCopyYAMLString(kv.first);
				float value = kv.second.as<float>();
				material->SetFloat(name, value);
			}

			const YAML::Node& float2sNode = node["Float2s"];
			for (const auto& kv : float2sNode)
			{
				std::string name = YAMLUtil::DeepCopyYAMLString(kv.first);
				glm::vec2 value = YAMLUtil::Vec2FromYAML(kv.second);
				material->SetFloat2(name, value);
			}

			const YAML::Node& float3sNode = node["Float3s"];
			for (const auto& kv : float3sNode)
			{
				std::string name = YAMLUtil::DeepCopyYAMLString(kv.first);
				glm::vec3 value = YAMLUtil::Vec3FromYAML(kv.second);
				material->SetFloat3(name, value);
			}

			const YAML::Node& float4sNode = node["Float4s"];
			for (const auto& kv : float4sNode)
			{
				std::string name = YAMLUtil::DeepCopyYAMLString(kv.first);
				glm::vec4 value = YAMLUtil::Vec4FromYAML(kv.second);
				material->SetFloat4(name, value);
			}

			const YAML::Node& mat4sNode = node["Mat4s"];
			for (const auto& kv : mat4sNode)
			{
				std::string name = YAMLUtil::DeepCopyYAMLString(kv.first);
				glm::mat4 value = YAMLUtil::Mat4FromYAML(kv.second);
				material->SetMat4(name, value);
			}

			const YAML::Node& texturesNode = node["Textures"];
			for (const auto& kv : texturesNode)
			{
				std::string name = YAMLUtil::DeepCopyYAMLString(kv.first);
				uint64_t value = YAMLUtil::GetType<uint64_t>(kv.second);
				material->textureIDs.push_back({ value, name });
			}

			material->shaderID = node["ShaderID"].as<uint64_t>();

			return material;
		}
	};

}
