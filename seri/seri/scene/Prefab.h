#pragma once

#include "seri/asset/AssetBase.h"

#include <yaml-cpp/yaml.h>

namespace seri::scene
{
	class Prefab : public seri::asset::AssetBase
	{
	public:
		YAML::Node entities{};

	};
}
