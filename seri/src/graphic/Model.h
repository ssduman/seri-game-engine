#pragma once

#include "util/Util.h"
#include "random/Random.h"
#include "asset/AssetBase.h"
#include "graphic/Mesh.h"

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

		int materialCount{ 0 };
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
