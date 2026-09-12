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
	YAML::Node IDComponent::Serialize(const IDComponent& component)
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

		if (node["IsActive"])
		{
			component.isActive = YAMLUtil::GetType<bool>(node["IsActive"]);
		}

		return component;
	}
	YAML::Node TransformComponent::Serialize(const TransformComponent& component)
	{
		YAML::Node node;
		node["Position"] = YAMLUtil::Vec3ToYAML(component.position);
		node["Rotation"] = YAMLUtil::Vec3ToYAML(component.rotation);
		node["Scale"] = YAMLUtil::Vec3ToYAML(component.scale);
		node["IsActive"] = component.isActive;
		return node;
	}

	SceneComponent SceneComponent::Deserialize(const YAML::Node& node)
	{
		SceneComponent component{};
		component.version = YAMLUtil::DeepCopyYAMLString(node["Version"]);
		component.isActive = YAMLUtil::GetType<bool>(node["IsActive"]);
		return component;
	}
	YAML::Node SceneComponent::Serialize(const SceneComponent& component)
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
	YAML::Node MeshComponent::Serialize(const MeshComponent& component)
	{
		YAML::Node node;
		node["MeshAssetID"] = component.meshAssetId;
		return node;
	}

	MeshRendererComponent MeshRendererComponent::Deserialize(const YAML::Node& node)
	{
		MeshRendererComponent component{};

		const YAML::Node& materialsNode = node["MaterialAssetIDs"];
		if (materialsNode && materialsNode.IsSequence())
		{
			for (const auto& materialNode : materialsNode)
			{
				component.materialAssetIds.emplace_back(YAMLUtil::GetType<uint64_t>(materialNode));
			}
		}
		else if (node["MaterialAssetID"])
		{
			component.materialAssetIds.emplace_back(YAMLUtil::GetType<uint64_t>(node["MaterialAssetID"]));
		}

		component.castShadow = YAMLUtil::GetType<bool>(node["CastShadow"]);

		return component;
	}
	YAML::Node MeshRendererComponent::Serialize(const MeshRendererComponent& component)
	{
		YAML::Node materialsNode;
		for (uint64_t materialAssetId : component.materialAssetIds)
		{
			materialsNode.push_back(materialAssetId);
		}

		YAML::Node node;
		node["MaterialAssetIDs"] = materialsNode;
		node["CastShadow"] = component.castShadow;
		return node;
	}

	SkinnedMeshRendererComponent SkinnedMeshRendererComponent::Deserialize(const YAML::Node& node)
	{
		SkinnedMeshRendererComponent component{};
		component.meshAssetId = YAMLUtil::GetType<uint64_t>(node["MeshAssetID"]);

		const YAML::Node& materialsNode = node["MaterialAssetIDs"];
		if (materialsNode && materialsNode.IsSequence())
		{
			for (const auto& materialNode : materialsNode)
			{
				component.materialAssetIds.emplace_back(YAMLUtil::GetType<uint64_t>(materialNode));
			}
		}

		if (node["CastShadow"])
		{
			component.castShadow = YAMLUtil::GetType<bool>(node["CastShadow"]);
		}

		return component;
	}
	YAML::Node SkinnedMeshRendererComponent::Serialize(const SkinnedMeshRendererComponent& component)
	{
		YAML::Node materialsNode;
		for (uint64_t materialAssetId : component.materialAssetIds)
		{
			materialsNode.push_back(materialAssetId);
		}

		YAML::Node node;
		node["MeshAssetID"] = component.meshAssetId;
		node["MaterialAssetIDs"] = materialsNode;
		node["CastShadow"] = component.castShadow;
		return node;
	}

	CanvasComponent CanvasComponent::Deserialize(const YAML::Node& node)
	{
		CanvasComponent component{};
		component.mode = static_cast<CanvasRenderMode>(YAMLUtil::GetType<int>(node["Mode"]));
		component.sortOrder = YAMLUtil::GetType<int>(node["SortOrder"]);
		return component;
	}
	YAML::Node CanvasComponent::Serialize(const CanvasComponent& component)
	{
		YAML::Node node;
		node["Mode"] = static_cast<int>(component.mode);
		node["SortOrder"] = component.sortOrder;
		return node;
	}

	RectComponent RectComponent::Deserialize(const YAML::Node& node)
	{
		RectComponent component{};
		component.anchorMin = YAMLUtil::Vec2FromYAML(node["AnchorMin"]);
		component.anchorMax = YAMLUtil::Vec2FromYAML(node["AnchorMax"]);
		component.pivot = YAMLUtil::Vec2FromYAML(node["Pivot"]);
		component.anchoredPosition = YAMLUtil::Vec2FromYAML(node["AnchoredPosition"]);
		component.sizeDelta = YAMLUtil::Vec2FromYAML(node["SizeDelta"]);
		return component;
	}
	YAML::Node RectComponent::Serialize(const RectComponent& component)
	{
		YAML::Node node;
		node["AnchorMin"] = YAMLUtil::Vec2ToYAML(component.anchorMin);
		node["AnchorMax"] = YAMLUtil::Vec2ToYAML(component.anchorMax);
		node["Pivot"] = YAMLUtil::Vec2ToYAML(component.pivot);
		node["AnchoredPosition"] = YAMLUtil::Vec2ToYAML(component.anchoredPosition);
		node["SizeDelta"] = YAMLUtil::Vec2ToYAML(component.sizeDelta);
		return node;
	}

	SpriteRendererComponent SpriteRendererComponent::Deserialize(const YAML::Node& node)
	{
		SpriteRendererComponent component{};
		component.textureAssetId = YAMLUtil::GetType<uint64_t>(node["TextureAssetID"]);
		component.color = YAMLUtil::Vec4FromYAML(node["Color"]);
		component.flipX = YAMLUtil::GetType<bool>(node["FlipX"]);
		component.flipY = YAMLUtil::GetType<bool>(node["FlipY"]);
		return component;
	}
	YAML::Node SpriteRendererComponent::Serialize(const SpriteRendererComponent& component)
	{
		YAML::Node node;
		node["TextureAssetID"] = component.textureAssetId;
		node["Color"] = YAMLUtil::Vec4ToYAML(component.color);
		node["FlipX"] = component.flipX;
		node["FlipY"] = component.flipY;
		return node;
	}

	AnimatorComponent AnimatorComponent::Deserialize(const YAML::Node& node)
	{
		AnimatorComponent component{};
		component.playing = YAMLUtil::GetType<bool>(node["Playing"]);
		component.loop = YAMLUtil::GetType<bool>(node["Loop"]);
		component.speed = YAMLUtil::GetType<float>(node["Speed"]);
		return component;
	}
	YAML::Node AnimatorComponent::Serialize(const AnimatorComponent& component)
	{
		YAML::Node node;
		node["Playing"] = component.playing;
		node["Loop"] = component.loop;
		node["Speed"] = component.speed;
		return node;
	}

	CameraComponent CameraComponent::Deserialize(const YAML::Node& node)
	{
		CameraComponent component{};
		component.isMain = YAMLUtil::GetType<bool>(node["IsMain"]);
		component.isOrtho = YAMLUtil::GetType<bool>(node["IsOrtho"]);
		component.fov = YAMLUtil::GetType<float>(node["FoV"]);
		component.orthoSize = YAMLUtil::GetType<float>(node["OrthoSize"]);
		component.nearPlane = YAMLUtil::GetType<float>(node["NearPlane"]);
		component.farPlane = YAMLUtil::GetType<float>(node["FarPlane"]);
		return component;
	}
	YAML::Node CameraComponent::Serialize(const CameraComponent& component)
	{
		YAML::Node node;
		node["IsMain"] = component.isMain;
		node["IsOrtho"] = component.isOrtho;
		node["FoV"] = component.fov;
		node["OrthoSize"] = component.orthoSize;
		node["NearPlane"] = component.nearPlane;
		node["FarPlane"] = component.farPlane;
		return node;
	}

	DirectionalLightComponent DirectionalLightComponent::Deserialize(const YAML::Node& node)
	{
		DirectionalLightComponent component{};
		component.intensity = YAMLUtil::GetType<float>(node["Intensity"]);
		component.color = YAMLUtil::Vec3FromYAML(node["Color"]);
		return component;
	}
	YAML::Node DirectionalLightComponent::Serialize(const DirectionalLightComponent& component)
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
	YAML::Node SpotLightComponent::Serialize(const SpotLightComponent& component)
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
	YAML::Node PointLightComponent::Serialize(const PointLightComponent& component)
	{
		YAML::Node node;
		node["Range"] = component.range;
		node["Intensity"] = component.intensity;
		node["Color"] = YAMLUtil::Vec3ToYAML(component.color);
		return node;
	}

	TextComponent TextComponent::Deserialize(const YAML::Node& node)
	{
		TextComponent component{};
		component.fontAssetId = YAMLUtil::GetType<uint64_t>(node["FontAssetID"]);
		component.text = YAMLUtil::DeepCopyYAMLString(node["Text"]);
		component.fontSize = YAMLUtil::GetType<float>(node["FontSize"]);
		component.lineSpacing = YAMLUtil::GetType<float>(node["LineSpacing"]);
		component.color = YAMLUtil::Vec4FromYAML(node["Color"]);
		component.alignH = static_cast<seri::font::TextAlignH>(YAMLUtil::GetType<int>(node["AlignH"]));
		component.alignV = static_cast<seri::font::TextAlignV>(YAMLUtil::GetType<int>(node["AlignV"]));
		return component;
	}
	YAML::Node TextComponent::Serialize(const TextComponent& component)
	{
		YAML::Node node;
		node["FontAssetID"] = component.fontAssetId;
		node["Text"] = component.text;
		node["FontSize"] = component.fontSize;
		node["LineSpacing"] = component.lineSpacing;
		node["Color"] = YAMLUtil::Vec4ToYAML(component.color);
		node["AlignH"] = static_cast<int>(component.alignH);
		node["AlignV"] = static_cast<int>(component.alignV);
		return node;
	}

	AudioComponent AudioComponent::Deserialize(const YAML::Node& node)
	{
		AudioComponent component{};
		component.soundPath = YAMLUtil::GetType<std::string>(node["SoundPath"]);
		component.volume = YAMLUtil::GetType<float>(node["Volume"]);
		component.loop = YAMLUtil::GetType<bool>(node["Loop"]);
		component.playOnStart = YAMLUtil::GetType<bool>(node["PlayOnStart"]);
		return component;
	}
	YAML::Node AudioComponent::Serialize(const AudioComponent& component)
	{
		YAML::Node node;
		node["SoundPath"] = component.soundPath;
		node["Volume"] = component.volume;
		node["Loop"] = component.loop;
		node["PlayOnStart"] = component.playOnStart;
		return node;
	}

	ScriptComponent ScriptComponent::Deserialize(const YAML::Node& node)
	{
		ScriptComponent component{};

		const YAML::Node& scriptsNode = node["Scripts"];
		if (scriptsNode && scriptsNode.IsSequence())
		{
			for (const auto& scriptNode : scriptsNode)
			{
				ScriptComponent::Entry entry{};

				if (scriptNode["Name"])
				{
					entry.name = YAMLUtil::DeepCopyYAMLString(scriptNode["Name"]);
				}
				if (scriptNode["Enabled"])
				{
					entry.enabled = YAMLUtil::GetType<bool>(scriptNode["Enabled"]);
				}
				if (scriptNode["Fields"] && scriptNode["Fields"].IsMap())
				{
					entry.fields = YAML::Clone(scriptNode["Fields"]);
				}

				component.entries.emplace_back(std::move(entry));
			}
		}

		return component;
	}
	YAML::Node ScriptComponent::Serialize(const ScriptComponent& component)
	{
		YAML::Node scriptsNode;
		for (const auto& entry : component.entries)
		{
			YAML::Node scriptNode;
			scriptNode["Name"] = entry.name;
			scriptNode["Enabled"] = entry.enabled;
			if (entry.fields.IsMap() && entry.fields.size() > 0)
			{
				scriptNode["Fields"] = entry.fields;
			}
			scriptsNode.push_back(scriptNode);
		}

		YAML::Node node;
		node["Scripts"] = scriptsNode;
		return node;
	}

}
