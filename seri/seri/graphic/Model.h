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

		int materialCount{ 0 };
		float importScale{ 1.0f };
		std::vector<std::shared_ptr<Mesh>> meshes{};

		void SetImportScale(float scale)
		{
			if (scale <= 0.0f || scale == importScale)
			{
				return;
			}

			glm::mat4 delta = glm::scale(glm::mat4{ 1.0f }, glm::vec3{ scale / importScale });
			for (const auto& mesh : meshes)
			{
				mesh->transformation = delta * mesh->transformation;
			}

			importScale = scale;
		}

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

		double GetAnimationDuration() const
		{
			double duration = 0.0;
			for (const auto& mesh : meshes)
			{
				if (mesh->animation.tickPerSecond <= 0.0)
				{
					continue;
				}

				double meshDuration = mesh->animation.durationInTick / mesh->animation.tickPerSecond;
				if (meshDuration > duration)
				{
					duration = meshDuration;
				}
			}
			return duration;
		}

	private:

	};
}
