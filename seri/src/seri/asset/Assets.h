#pragma once

#include "seri/util/Util.h"
#include "seri/util/YAMLUtil.h"
#include "seri/graphic/Model.h"
#include "seri/graphic/Material.h"
#include "seri/model/ModelImporter.h"

namespace seri::asset
{
	struct IDInfo
	{
		uint64_t id{ 0 };
		std::string version{ "0.1" };

		static YAML::Node Serialize(const IDInfo& asset)
		{
			YAML::Node node;
			node["ID"] = asset.id;
			node["Version"] = asset.version;
			return node;
		}

		static IDInfo Deserialize(const YAML::Node& root)
		{
			IDInfo component;
			component.id = root["ID"].as<uint64_t>();
			component.version = YAMLUtil::DeepCopyYAMLString(root["Version"]);
			return component;
		}
	};

	struct MaterialAsset
	{
		static YAML::Node Serialize(const std::shared_ptr<Material>& asset)
		{
			YAML::Node root;

			YAML::Node intsNode;
			for (const auto& kv : asset->GetInts())
			{
				intsNode[kv.first] = kv.second;
			}
			root["Ints"] = intsNode;

			YAML::Node floatsNode;
			for (const auto& kv : asset->GetFloats())
			{
				floatsNode[kv.first] = kv.second;
			}
			root["Floats"] = floatsNode;

			YAML::Node float2sNode;
			for (const auto& kv : asset->GetFloat2s())
			{
				float2sNode[kv.first] = YAMLUtil::Vec2ToYAML(kv.second);
			}
			root["Float2s"] = float2sNode;

			YAML::Node float3sNode;
			for (const auto& kv : asset->GetFloat3s())
			{
				float3sNode[kv.first] = YAMLUtil::Vec3ToYAML(kv.second);
			}
			root["Float3s"] = float3sNode;

			YAML::Node float4sNode;
			for (const auto& kv : asset->GetFloat4s())
			{
				float4sNode[kv.first] = YAMLUtil::Vec4ToYAML(kv.second);
			}
			root["Float4s"] = float4sNode;

			YAML::Node mat4sNode;
			for (const auto& kv : asset->GetMats())
			{
				if (kv.first.starts_with("u_bones"))
				{
					continue;
				}
				mat4sNode[kv.first] = YAMLUtil::Mat4ToYAML(kv.second);
			}
			root["Mat4s"] = mat4sNode;

			YAML::Node texturesNode;
			for (const auto& kv : asset->GetTextures())
			{
				texturesNode[kv.first] = kv.second->id;
			}
			root["Textures"] = texturesNode;

			root["ShaderID"] = asset->shader != nullptr ? asset->shader->id : 0;

			return root;
		}

		static std::shared_ptr<Material> Deserialize(const YAML::Node& root)
		{
			std::shared_ptr<Material> asset = std::make_shared<Material>();

			const YAML::Node& intsNode = root["Ints"];
			for (const auto& kv : intsNode)
			{
				std::string name = YAMLUtil::DeepCopyYAMLString(kv.first);
				int value = kv.second.as<int>();
				asset->SetInt(name, value);
			}

			const YAML::Node& floatsNode = root["Floats"];
			for (const auto& kv : floatsNode)
			{
				std::string name = YAMLUtil::DeepCopyYAMLString(kv.first);
				float value = kv.second.as<float>();
				asset->SetFloat(name, value);
			}

			const YAML::Node& float2sNode = root["Float2s"];
			for (const auto& kv : float2sNode)
			{
				std::string name = YAMLUtil::DeepCopyYAMLString(kv.first);
				glm::vec2 value = YAMLUtil::Vec2FromYAML(kv.second);
				asset->SetFloat2(name, value);
			}

			const YAML::Node& float3sNode = root["Float3s"];
			for (const auto& kv : float3sNode)
			{
				std::string name = YAMLUtil::DeepCopyYAMLString(kv.first);
				glm::vec3 value = YAMLUtil::Vec3FromYAML(kv.second);
				asset->SetFloat3(name, value);
			}

			const YAML::Node& float4sNode = root["Float4s"];
			for (const auto& kv : float4sNode)
			{
				std::string name = YAMLUtil::DeepCopyYAMLString(kv.first);
				glm::vec4 value = YAMLUtil::Vec4FromYAML(kv.second);
				asset->SetFloat4(name, value);
			}

			const YAML::Node& mat4sNode = root["Mat4s"];
			for (const auto& kv : mat4sNode)
			{
				std::string name = YAMLUtil::DeepCopyYAMLString(kv.first);
				glm::mat4 value = YAMLUtil::Mat4FromYAML(kv.second);
				asset->SetMat4(name, value);
			}

			const YAML::Node& texturesNode = root["Textures"];
			for (const auto& kv : texturesNode)
			{
				std::string name = YAMLUtil::DeepCopyYAMLString(kv.first);
				uint64_t value = YAMLUtil::GetType<uint64_t>(kv.second);
				asset->textureIDs.push_back({ value, name });
			}

			asset->shaderID = root["ShaderID"].as<uint64_t>();

			return asset;
		}
	};

	struct MeshAsset
	{
		static YAML::Node Serialize(const std::shared_ptr<Model>& model)
		{
			YAML::Node root;

			root["MaterialCount"] = model->materialCount;

			YAML::Node meshesNode;
			for (const auto& mesh : model->meshes)
			{
				YAML::Node meshNode;
				meshNode["Name"] = mesh->name;
				meshNode["MaterialIndex"] = mesh->materialIndex;
				meshesNode.push_back(meshNode);
			}
			root["Meshes"] = meshesNode;

			return root;
		}

		static std::shared_ptr<Model> Deserialize(const YAML::Node& root, seri::asset::AssetMetadata metadata)
		{
			std::shared_ptr<Model> asset = seri::ModelImporter{}.Load(metadata.source.string());

			asset->materialCount = YAMLUtil::GetType<int>(root["MaterialCount"]);

			const YAML::Node& meshesNode = root["Meshes"];
			for (const auto& meshNode : meshesNode)
			{
				std::string name = YAMLUtil::DeepCopyYAMLString(meshNode["Name"]);
				int materialIndex = YAMLUtil::GetType<int>(meshNode["MaterialIndex"]);
				for (const auto& mesh : asset->meshes)
				{
					if (mesh->name == name)
					{
						mesh->materialIndex = materialIndex;
						break;
					}
				}
			}

			return asset;
		}
	};

}
