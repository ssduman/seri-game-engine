#include "Seripch.h"

#include "seri/asset/Assets.h"

#include "seri/util/Util.h"
#include "seri/util/YAMLUtil.h"
#include "seri/graphic/Model.h"
#include "seri/graphic/Material.h"
#include "seri/model/ModelImporter.h"
#include "seri/shader/ShaderBase.h"

#include <yaml-cpp/yaml.h>

#include <memory>
#include <algorithm>

namespace seri::asset
{
	YAML::Node IDInfo::Serialize(const IDInfo& asset)
	{
		YAML::Node node;
		node["ID"] = asset.id;
		node["Version"] = asset.version;
		return node;
	}

	IDInfo IDInfo::Deserialize(const YAML::Node& root)
	{
		IDInfo component;
		component.id = root["ID"].as<uint64_t>();
		component.version = YAMLUtil::DeepCopyYAMLString(root["Version"]);
		return component;
	}

	YAML::Node MaterialAsset::Serialize(const std::shared_ptr<Material>& asset)
	{
		YAML::Node root;

		YAML::Node boolsNode;
		for (const auto& kv : asset->GetBools())
		{
			boolsNode[kv.first] = kv.second;
		}
		root["Bools"] = boolsNode;

		YAML::Node intsNode;
		for (const auto& kv : asset->GetInts())
		{
			intsNode[kv.first] = kv.second;
		}
		root["Ints"] = intsNode;

		YAML::Node int2sNode;
		for (const auto& kv : asset->GetInt2s())
		{
			int2sNode[kv.first] = YAMLUtil::IVec2ToYAML(kv.second);
		}
		root["Int2s"] = int2sNode;

		YAML::Node int3sNode;
		for (const auto& kv : asset->GetInt3s())
		{
			int3sNode[kv.first] = YAMLUtil::IVec3ToYAML(kv.second);
		}
		root["Int3s"] = int3sNode;

		YAML::Node int4sNode;
		for (const auto& kv : asset->GetInt4s())
		{
			int4sNode[kv.first] = YAMLUtil::IVec4ToYAML(kv.second);
		}
		root["Int4s"] = int4sNode;

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
			if (Util::IsIgnoredUniform(kv.first))
			{
				continue;
			}
			float3sNode[kv.first] = YAMLUtil::Vec3ToYAML(kv.second);
		}
		root["Float3s"] = float3sNode;

		YAML::Node float4sNode;
		for (const auto& kv : asset->GetFloat4s())
		{
			if (Util::IsIgnoredUniform(kv.first))
			{
				continue;
			}
			float4sNode[kv.first] = YAMLUtil::Vec4ToYAML(kv.second);
		}
		root["Float4s"] = float4sNode;

		YAML::Node mat4sNode;
		for (const auto& kv : asset->GetMats())
		{
			if (Util::IsIgnoredUniform(kv.first))
			{
				continue;
			}
			mat4sNode[kv.first] = YAMLUtil::Mat4ToYAML(kv.second);
		}
		root["Mat4s"] = mat4sNode;

		YAML::Node texturesNode;
		for (const auto& kv : asset->GetTextures())
		{
			texturesNode[kv.first] = kv.second ? kv.second->id : 0;
		}
		root["Textures"] = texturesNode;

		root["ShaderID"] = asset->GetShader() != nullptr ? asset->GetShader()->id : 0;

		return root;
	}

	std::shared_ptr<Material> MaterialAsset::Deserialize(const YAML::Node& root)
	{
		std::shared_ptr<Material> asset = std::make_shared<Material>();

		const YAML::Node& boolsNode = root["Bools"];
		for (const auto& kv : boolsNode)
		{
			std::string name = YAMLUtil::DeepCopyYAMLString(kv.first);
			bool value = YAMLUtil::GetType<bool>(kv.second);
			asset->SetBool(name, value);
		}

		const YAML::Node& intsNode = root["Ints"];
		for (const auto& kv : intsNode)
		{
			std::string name = YAMLUtil::DeepCopyYAMLString(kv.first);
			int value = YAMLUtil::GetType<int>(kv.second);
			asset->SetInt(name, value);
		}

		const YAML::Node& int2sNode = root["Int2s"];
		for (const auto& kv : int2sNode)
		{
			std::string name = YAMLUtil::DeepCopyYAMLString(kv.first);
			glm::ivec2 value = YAMLUtil::IVec2FromYAML(kv.second);
			asset->SetInt2(name, value);
		}

		const YAML::Node& int3sNode = root["Int3s"];
		for (const auto& kv : int3sNode)
		{
			std::string name = YAMLUtil::DeepCopyYAMLString(kv.first);
			glm::ivec3 value = YAMLUtil::IVec3FromYAML(kv.second);
			asset->SetInt3(name, value);
		}

		const YAML::Node& int4sNode = root["Int4s"];
		for (const auto& kv : int4sNode)
		{
			std::string name = YAMLUtil::DeepCopyYAMLString(kv.first);
			glm::ivec4 value = YAMLUtil::IVec4FromYAML(kv.second);
			asset->SetInt4(name, value);
		}

		const YAML::Node& floatsNode = root["Floats"];
		for (const auto& kv : floatsNode)
		{
			std::string name = YAMLUtil::DeepCopyYAMLString(kv.first);
			float value = YAMLUtil::GetType<float>(kv.second);
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

	YAML::Node MeshAsset::Serialize(const std::shared_ptr<Model>& model)
	{
		YAML::Node root;

		root["MaterialCount"] = model->materialCount;

		YAML::Node meshesNode;
		for (const auto& mesh : model->meshes)
		{
			YAML::Node meshNode;
			meshNode["MeshName"] = mesh->name;
			meshNode["MaterialName"] = mesh->materialName;
			meshNode["MaterialIndex"] = mesh->materialIndex;
			meshesNode.push_back(meshNode);
		}
		root["Meshes"] = meshesNode;

		return root;
	}

	std::shared_ptr<Model> MeshAsset::Deserialize(const YAML::Node& root, seri::asset::AssetMetadata metadata)
	{
		std::shared_ptr<Model> asset = seri::ModelImporter{}.Load(metadata.source.string());

		asset->materialCount = YAMLUtil::GetType<int>(root["MaterialCount"]);

		const YAML::Node& meshesNode = root["Meshes"];
		for (const auto& meshNode : meshesNode)
		{
			std::string meshName = YAMLUtil::DeepCopyYAMLString(meshNode["MeshName"]);
			std::string materialName = YAMLUtil::DeepCopyYAMLString(meshNode["MaterialName"]);
			int materialIndex = YAMLUtil::GetType<int>(meshNode["MaterialIndex"]);
			for (const auto& mesh : asset->meshes)
			{
				if (mesh->name == meshName)
				{
					mesh->materialName = materialName;
					mesh->materialIndex = materialIndex;
					break;
				}
			}
		}

		return asset;
	}
}
