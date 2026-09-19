#pragma once

#include "seri/util/Util.h"
#include "seri/util/YAMLUtil.h"
#include "seri/font/TextMesh.h"
#include "seri/ui/UIUtil.h"
#include "seri/physics/PhysicsUtil.h"

#include <entt/entt.hpp>
#include <yaml-cpp/yaml.h>

#include <string>
#include <vector>
#include <memory>

namespace seri
{
	class Mesh;
	class Material;
}

namespace seri::component
{
	struct IDComponent
	{
		static constexpr std::string_view kCompName = "IDComponent";

		uint64_t id{ 0 };
		uint64_t parentId{ 0 };
		std::string name{ "" };

		static IDComponent Deserialize(const YAML::Node& node);
		static YAML::Node Serialize(const IDComponent& component);
	};

	struct TransformComponent
	{
		static constexpr std::string_view kCompName = "TransformComponent";

		glm::vec3 position{ 0.0f, 0.0f, 0.0f };
		glm::vec3 rotation{ 0.0f, 0.0f, 0.0f };
		glm::vec3 scale{ 1.0f, 1.0f, 1.0f };
		bool isActive{ true };

		glm::mat4 localMatrix{ 1.0f };
		glm::mat4 worldMatrix{ 1.0f };
		bool isActiveInHierarchy{ true };

		static TransformComponent Deserialize(const YAML::Node& node);
		static YAML::Node Serialize(const TransformComponent& component);
	};

	struct SceneComponent
	{
		static constexpr std::string_view kCompName = "SceneComponent";

		std::string version{ "0.0" };
		bool isActive{ true };

		static SceneComponent Deserialize(const YAML::Node& node);
		static YAML::Node Serialize(const SceneComponent& component);
	};

	struct MeshComponent
	{
		static constexpr std::string_view kCompName = "MeshComponent";

		uint64_t meshAssetId{ 0 };

		static MeshComponent Deserialize(const YAML::Node& node);
		static YAML::Node Serialize(const MeshComponent& component);
	};

	struct MeshRendererComponent
	{
		static constexpr std::string_view kCompName = "MeshRendererComponent";

		std::vector<uint64_t> materialAssetIds{};
		bool castShadow = true;

		static MeshRendererComponent Deserialize(const YAML::Node& node);
		static YAML::Node Serialize(const MeshRendererComponent& component);
	};

	struct SkinnedMeshRendererComponent
	{
		static constexpr std::string_view kCompName = "SkinnedMeshRendererComponent";

		uint64_t meshAssetId{ 0 };
		std::vector<uint64_t> materialAssetIds{};
		bool castShadow = true;

		static SkinnedMeshRendererComponent Deserialize(const YAML::Node& node);
		static YAML::Node Serialize(const SkinnedMeshRendererComponent& component);
	};

	struct CanvasComponent
	{
		static constexpr std::string_view kCompName = "CanvasComponent";

		seri::ui::CanvasRenderMode mode{ seri::ui::CanvasRenderMode::screen_space };
		int sortOrder{ 0 };

		static CanvasComponent Deserialize(const YAML::Node& node);
		static YAML::Node Serialize(const CanvasComponent& component);
	};

	struct RectComponent
	{
		static constexpr std::string_view kCompName = "RectComponent";

		glm::vec2 anchorMin{ 0.5f, 0.5f };
		glm::vec2 anchorMax{ 0.5f, 0.5f };
		glm::vec2 pivot{ 0.5f, 0.5f };
		glm::vec2 anchoredPosition{ 0.0f, 0.0f };
		glm::vec2 sizeDelta{ 100.0f, 100.0f };

		glm::vec2 resolvedSize{ 0.0f, 0.0f };
		glm::vec2 resolvedAnchorMinPos{ 0.0f, 0.0f };
		glm::vec2 resolvedAnchorMaxPos{ 0.0f, 0.0f };
		glm::mat4 resolvedParentFrame{ 1.0f };

		static RectComponent Deserialize(const YAML::Node& node);
		static YAML::Node Serialize(const RectComponent& component);
	};

	struct ButtonComponent
	{
		static constexpr std::string_view kCompName = "ButtonComponent";

		bool interactable{ true };
		glm::vec4 normalColor{ 1.0f, 1.0f, 1.0f, 1.0f };
		glm::vec4 hoverColor{ 0.9f, 0.9f, 0.9f, 1.0f };
		glm::vec4 pressedColor{ 0.7f, 0.7f, 0.7f, 1.0f };
		glm::vec4 disabledColor{ 0.5f, 0.5f, 0.5f, 0.5f };

		bool hovered{ false };
		bool pressed{ false };
		bool clicked{ false };

		glm::vec4 GetTint() const
		{
			if (!interactable)
			{
				return disabledColor;
			}
			if (pressed && hovered)
			{
				return pressedColor;
			}
			if (hovered)
			{
				return hoverColor;
			}
			return normalColor;
		}

		static ButtonComponent Deserialize(const YAML::Node& node);
		static YAML::Node Serialize(const ButtonComponent& component);
	};

	struct SpriteRendererComponent
	{
		static constexpr std::string_view kCompName = "SpriteRendererComponent";

		uint64_t textureAssetId{ 0 };
		glm::vec4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
		bool flipX{ false };
		bool flipY{ false };

		struct BuiltState
		{
			bool flipX{ false };
			bool flipY{ false };
			glm::vec2 size{ 0.0f, 0.0f };
			glm::vec2 pivot{ 0.0f, 0.0f };

			bool operator==(const BuiltState& other) const = default;
		};

		std::shared_ptr<seri::Mesh> mesh{};
		std::shared_ptr<seri::Material> material{};
		BuiltState built{};

		static SpriteRendererComponent Deserialize(const YAML::Node& node);
		static YAML::Node Serialize(const SpriteRendererComponent& component);
	};

	struct AnimatorComponent
	{
		static constexpr std::string_view kCompName = "AnimatorComponent";

		bool playing{ true };
		bool loop{ true };
		float speed{ 1.0f };

		float time{ 0.0f };

		static AnimatorComponent Deserialize(const YAML::Node& node);
		static YAML::Node Serialize(const AnimatorComponent& component);
	};

	struct CameraComponent
	{
		static constexpr std::string_view kCompName = "CameraComponent";

		bool isMain{ true };
		bool isOrtho{ false };
		float fov{ 60.0f };
		float orthoSize{ 5.0f };
		float nearPlane{ 0.1f };
		float farPlane{ 1000.0f };

		static CameraComponent Deserialize(const YAML::Node& node);
		static YAML::Node Serialize(const CameraComponent& component);
	};

	struct DirectionalLightComponent
	{
		static constexpr std::string_view kCompName = "DirectionalLightComponent";

		float intensity = 1.0f;
		glm::vec3 color{ 1.0f, 1.0f, 1.0f };

		static DirectionalLightComponent Deserialize(const YAML::Node& node);
		static YAML::Node Serialize(const DirectionalLightComponent& component);
	};

	struct SpotLightComponent
	{
		static constexpr std::string_view kCompName = "SpotLightComponent";

		float intensity = 10.0f;
		float innerAngle = 20.0f;
		float outerAngle = 30.0f;
		bool castShadow = false;
		glm::vec3 color{ 1.0f, 1.0f, 1.0f };

		float constant = 1.0f;
		float linear = 0.09f;
		float quadratic = 0.032f;

		static SpotLightComponent Deserialize(const YAML::Node& node);
		static YAML::Node Serialize(const SpotLightComponent& component);
	};

	struct PointLightComponent
	{
		static constexpr std::string_view kCompName = "PointLightComponent";

		float range = 10.0f;
		float intensity = 10.0f;
		glm::vec3 color{ 1.0f, 1.0f, 1.0f };

		float constant = 1.0f;
		float linear = 0.09f;
		float quadratic = 0.032f;

		static PointLightComponent Deserialize(const YAML::Node& node);
		static YAML::Node Serialize(const PointLightComponent& component);
	};

	struct TextComponent
	{
		static constexpr std::string_view kCompName = "TextComponent";

		uint64_t fontAssetId{ 0 };
		std::string text{ "New Text" };
		float fontSize{ 1.0f };
		float lineSpacing{ 1.0f };
		glm::vec4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
		seri::font::TextAlignH alignH{ seri::font::TextAlignH::center };
		seri::font::TextAlignV alignV{ seri::font::TextAlignV::middle };

		struct BuiltState
		{
			uint64_t fontAssetId{ 0 };
			std::string text{ "" };
			float fontSize{ 0.0f };
			float lineSpacing{ 0.0f };
			seri::font::TextAlignH alignH{};
			seri::font::TextAlignV alignV{};

			bool operator==(const BuiltState& other) const = default;
		};

		std::shared_ptr<seri::Mesh> mesh{};
		std::shared_ptr<seri::Material> material{};
		BuiltState built{};

		static TextComponent Deserialize(const YAML::Node& node);
		static YAML::Node Serialize(const TextComponent& component);
	};

	struct AudioComponent
	{
		static constexpr std::string_view kCompName = "AudioComponent";

		uint64_t soundAssetId{ 0 };
		float volume{ 1.0f };
		bool loop{ false };
		bool playOnStart{ true };

		uint64_t handle{ 0 };
		uint64_t builtSoundAssetId{ 0 };
		bool started{ false };

		static AudioComponent Deserialize(const YAML::Node& node);
		static YAML::Node Serialize(const AudioComponent& component);
	};

	struct RigidbodyComponent
	{
		static constexpr std::string_view kCompName = "RigidbodyComponent";

		float mass{ 1.0f };
		float linearDamping{ 0.0f };
		float angularDamping{ 0.05f };
		float gravityScale{ 1.0f };
		bool isKinematic{ false };
		glm::bvec3 lockPosition{ false };
		glm::bvec3 lockRotation{ false };

		bool operator==(const RigidbodyComponent&) const = default;

		static RigidbodyComponent Deserialize(const YAML::Node& node);
		static YAML::Node Serialize(const RigidbodyComponent& component);
	};

	struct ColliderComponent
	{
		static constexpr std::string_view kCompName = "ColliderComponent";

		seri::physics::ColliderShape shape{ seri::physics::ColliderShape::box };
		glm::vec3 center{ 0.0f, 0.0f, 0.0f };
		glm::vec3 size{ 1.0f, 1.0f, 1.0f };
		float radius{ 0.5f };
		float height{ 2.0f };
		float friction{ 0.6f };
		float restitution{ 0.0f };
		bool isTrigger{ false };

		bool operator==(const ColliderComponent&) const = default;

		static ColliderComponent Deserialize(const YAML::Node& node);
		static YAML::Node Serialize(const ColliderComponent& component);
	};

	struct ScriptComponent
	{
		static constexpr std::string_view kCompName = "ScriptComponent";

		struct Entry
		{
			std::string name{ "" };
			bool enabled{ true };
			YAML::Node fields{};
		};

		std::vector<Entry> entries{};
		bool dirty{ true };

		static ScriptComponent Deserialize(const YAML::Node& node);
		static YAML::Node Serialize(const ScriptComponent& component);
	};

}
