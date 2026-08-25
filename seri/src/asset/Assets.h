#pragma once

#include "util/Util.h"
#include "util/YAMLUtil.h"
#include "graphic/Model.h"
#include "graphic/Material.h"
#include "model/ModelImporter.h"

namespace seri::asset
{
	struct IDInfo
	{
		uint64_t id{ 0 };
		std::string version{ "0.1" };

		static YAML::Node Serialize(const IDInfo& asset);

		static IDInfo Deserialize(const YAML::Node& root);
	};

	struct MaterialAsset
	{
		static YAML::Node Serialize(const std::shared_ptr<Material>& asset);

		static std::shared_ptr<Material> Deserialize(const YAML::Node& root);
	};

	struct MeshAsset
	{
		static YAML::Node Serialize(const std::shared_ptr<Model>& model);

		static std::shared_ptr<Model> Deserialize(const YAML::Node& root, seri::asset::AssetMetadata metadata);
	};

}
