#pragma once

#include "seri/util/Util.h"
#include "seri/util/YAMLUtil.h"
#include "seri/font/TextMesh.h"

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
		static constexpr std::string_view compName = "IDComponent";

		uint64_t id{ 0 };
		uint64_t parentId{ 0 };
		std::string name{ "" };

		static IDComponent Deserialize(const YAML::Node& node);
		static YAML::Node Serialize(const IDComponent& component);
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
		static YAML::Node Serialize(const TransformComponent& component);
	};

	struct SceneComponent
	{
		static constexpr std::string_view compName = "SceneComponent";

		std::string version{ "0.0" };
		bool isActive{ true };

		static SceneComponent Deserialize(const YAML::Node& node);
		static YAML::Node Serialize(const SceneComponent& component);
	};

	struct MeshComponent
	{
		static constexpr std::string_view compName = "MeshComponent";

		uint64_t meshAssetId{ 0 };

		static MeshComponent Deserialize(const YAML::Node& node);
		static YAML::Node Serialize(const MeshComponent& component);
	};

	struct MeshRendererComponent
	{
		static constexpr std::string_view compName = "MeshRendererComponent";

		std::vector<uint64_t> materialAssetIds{};
		bool castShadow = true;

		static MeshRendererComponent Deserialize(const YAML::Node& node);
		static YAML::Node Serialize(const MeshRendererComponent& component);
	};

	struct SkinnedMeshRendererComponent
	{
		static constexpr std::string_view compName = "SkinnedMeshRendererComponent";

		uint64_t meshAssetId{ 0 };
		std::vector<uint64_t> materialAssetIds{};

		static SkinnedMeshRendererComponent Deserialize(const YAML::Node& node);
		static YAML::Node Serialize(const SkinnedMeshRendererComponent& component);
	};

	struct AnimatorComponent
	{
		static constexpr std::string_view compName = "AnimatorComponent";

		bool playing{ true };
		bool loop{ true };
		float speed{ 1.0f };

		float time{ 0.0f };

		static AnimatorComponent Deserialize(const YAML::Node& node);
		static YAML::Node Serialize(const AnimatorComponent& component);
	};

	struct CameraComponent
	{
		static constexpr std::string_view compName = "CameraComponent";

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
		static constexpr std::string_view compName = "DirectionalLightComponent";

		float intensity = 1.0f;
		glm::vec3 color{ 1.0f, 1.0f, 1.0f };

		static DirectionalLightComponent Deserialize(const YAML::Node& node);
		static YAML::Node Serialize(const DirectionalLightComponent& component);
	};

	struct SpotLightComponent
	{
		static constexpr std::string_view compName = "SpotLightComponent";

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
		static constexpr std::string_view compName = "PointLightComponent";

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
		static constexpr std::string_view compName = "TextComponent";

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

	struct ScriptComponent
	{
		static constexpr std::string_view compName = "ScriptComponent";

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
