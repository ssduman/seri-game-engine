#include "Seripch.h"

#include "seri/component/Components.h"

namespace seri::component
{
	IDComponent IDComponent::Deserialize(const YAML::Node& node)
	{
		IDComponent component{};
		component.id = YAMLUtil::GetType<uint64_t>(node["ID"]);
		component.parentId = YAMLUtil::GetType<uint64_t>(node["ParentID"]);
		component.name = YAMLUtil::DeepCopyYAMLString(node["Name"]);
		return component;
	}
	YAML::Node IDComponent::Serialize(IDComponent component)
	{
		YAML::Node node;
		node["ID"] = component.id;
		node["ParentID"] = component.parentId;
		node["Name"] = component.name;
		return node;
	}

	TransformComponent TransformComponent::Deserialize(const YAML::Node& node)
	{
		TransformComponent component{};
		component.position = YAMLUtil::Vec3FromYAML(node["Position"]);
		component.rotation = YAMLUtil::Vec3FromYAML(node["Rotation"]);
		component.scale = YAMLUtil::Vec3FromYAML(node["Scale"]);
		return component;
	}
	YAML::Node TransformComponent::Serialize(TransformComponent component)
	{
		YAML::Node node;
		node["Position"] = YAMLUtil::Vec3ToYAML(component.position);
		node["Rotation"] = YAMLUtil::Vec3ToYAML(component.rotation);
		node["Scale"] = YAMLUtil::Vec3ToYAML(component.scale);
		return node;
	}

	SceneComponent SceneComponent::Deserialize(const YAML::Node& node)
	{
		SceneComponent component{};
		component.version = YAMLUtil::DeepCopyYAMLString(node["Version"]);
		component.isActive = YAMLUtil::GetType<bool>(node["IsActive"]);
		return component;
	}
	YAML::Node SceneComponent::Serialize(SceneComponent component)
	{
		YAML::Node node;
		node["Version"] = component.version;
		node["IsActive"] = component.isActive;
		return node;
	}

	MeshComponent MeshComponent::Deserialize(const YAML::Node& node)
	{
		MeshComponent component{};
		component.meshAssetId = YAMLUtil::GetType<uint64_t>(node["MeshAssetID"]);
		return component;
	}
	YAML::Node MeshComponent::Serialize(MeshComponent component)
	{
		YAML::Node node;
		node["MeshAssetID"] = component.meshAssetId;
		return node;
	}

	MeshRendererComponent MeshRendererComponent::Deserialize(const YAML::Node& node)
	{
		MeshRendererComponent component{};
		component.materialAssetId = YAMLUtil::GetType<uint64_t>(node["MaterialAssetID"]);
		component.castShadow = YAMLUtil::GetType<bool>(node["CastShadow"]);
		return component;
	}
	YAML::Node MeshRendererComponent::Serialize(MeshRendererComponent component)
	{
		YAML::Node node;
		node["MaterialAssetID"] = component.materialAssetId;
		node["CastShadow"] = component.castShadow;
		return node;
	}

	DirectionalLightComponent DirectionalLightComponent::Deserialize(const YAML::Node& node)
	{
		DirectionalLightComponent component{};
		component.intensity = YAMLUtil::GetType<float>(node["Intensity"]);
		component.color = YAMLUtil::Vec3FromYAML(node["Color"]);
		return component;
	}
	YAML::Node DirectionalLightComponent::Serialize(DirectionalLightComponent component)
	{
		YAML::Node node;
		node["Intensity"] = component.intensity;
		node["Color"] = YAMLUtil::Vec3ToYAML(component.color);
		return node;
	}

	SpotLightComponent SpotLightComponent::Deserialize(const YAML::Node& node)
	{
		SpotLightComponent component{};
		component.intensity = YAMLUtil::GetType<float>(node["Intensity"]);
		component.innerAngle = YAMLUtil::GetType<float>(node["InnerAngle"]);
		component.outerAngle = YAMLUtil::GetType<float>(node["OuterAngle"]);
		component.castShadow = YAMLUtil::GetType<bool>(node["CastShadow"]);
		component.color = YAMLUtil::Vec3FromYAML(node["Color"]);
		return component;
	}
	YAML::Node SpotLightComponent::Serialize(SpotLightComponent component)
	{
		YAML::Node node;
		node["Intensity"] = component.intensity;
		node["InnerAngle"] = component.innerAngle;
		node["OuterAngle"] = component.outerAngle;
		node["CastShadow"] = component.castShadow;
		node["Color"] = YAMLUtil::Vec3ToYAML(component.color);
		return node;
	}

	PointLightComponent PointLightComponent::Deserialize(const YAML::Node& node)
	{
		PointLightComponent component{};
		component.range = YAMLUtil::GetType<float>(node["Range"]);
		component.intensity = YAMLUtil::GetType<float>(node["Intensity"]);
		component.color = YAMLUtil::Vec3FromYAML(node["Color"]);
		return component;
	}
	YAML::Node PointLightComponent::Serialize(PointLightComponent component)
	{
		YAML::Node node;
		node["Range"] = component.range;
		node["Intensity"] = component.intensity;
		node["Color"] = YAMLUtil::Vec3ToYAML(component.color);
		return node;
	}

}
