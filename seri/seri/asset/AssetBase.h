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
	};

	inline const char* AssetTypeToString(AssetType type)
	{
		switch (type)
		{
			case AssetType::none: return "none";
			case AssetType::material: return "material";
			case AssetType::shader: return "shader";
			case AssetType::texture: return "texture";
			case AssetType::mesh: return "mesh";
			case AssetType::skybox: return "skybox";
			case AssetType::scene: return "scene";
			case AssetType::prefab: return "prefab";
			default: return "unknown";
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
