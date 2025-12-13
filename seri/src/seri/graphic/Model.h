#pragma once

#include "seri/util/Util.h"
#include "seri/random/Random.h"
#include "seri/asset/AssetBase.h"
#include "seri/graphic/Mesh.h"

#include <vector>
#include <memory>

namespace seri
{
	class Model : public seri::asset::AssetBase
	{
	public:
		Model()
		{
			id = seri::Random::UUID();
			type = seri::asset::AssetType::mesh;
		}

		Model(uint64_t id_)
		{
			id = id_;
			type = seri::asset::AssetType::mesh;
		}

		std::vector<std::shared_ptr<Mesh>> meshes{};

		void Build()
		{
			for (const auto& mesh : meshes)
			{
				mesh->Build();
			}
		}

		void UpdateAnimations(double time)
		{
			for (const auto& mesh : meshes)
			{
				mesh->UpdateAnimation(time);
			}
		}

	private:

	};
}
