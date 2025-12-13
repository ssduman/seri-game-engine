#pragma once

#include "seri/util/Util.h"

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

	struct AssetMetadata
	{
		uint64_t id{ 0 };
		AssetType type{ AssetType::none };
		std::filesystem::path source{};
		std::filesystem::path meta{};
	};

	class AssetBase
	{
	public:
		~AssetBase() = default;

		virtual void Copy()
		{
		}

		uint64_t id{ 0 };
		AssetType type{ AssetType::none };
	};
}
