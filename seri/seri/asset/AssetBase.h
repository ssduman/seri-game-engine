#pragma once

#include "seri/util/Util.h"

#include <string>
#include <filesystem>

namespace seri::asset
{
	enum class AssetType
	{
		none,
		material,
		shader,
		texture,
		mesh,
		skybox,
		scene,
		prefab,
		font,
		script,
		sound,
		animation_state_machine,
	};

	inline const char* AssetTypeToString(AssetType type)
	{
		switch (type)
		{
			case AssetType::none: return "None";
			case AssetType::material: return "Material";
			case AssetType::shader: return "Shader";
			case AssetType::texture: return "Texture";
			case AssetType::mesh: return "Mesh";
			case AssetType::skybox: return "Skybox";
			case AssetType::scene: return "Scene";
			case AssetType::prefab: return "Prefab";
			case AssetType::font: return "Font";
			case AssetType::script: return "Script";
			case AssetType::sound: return "Sound";
			case AssetType::animation_state_machine: return "Animation State Machine";
			default: return "Unknown";
		}
	}

	struct AssetMetadata
	{
		uint64_t id{ 0 };
		AssetType type{ AssetType::none };
		std::string name{ "" };
		std::filesystem::path source{};
		std::filesystem::path meta{};
	};

	class AssetBase
	{
	public:
		virtual ~AssetBase() = default;

		uint64_t id{ 0 };
		AssetType type{ AssetType::none };
	};
}
