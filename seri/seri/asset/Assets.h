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
