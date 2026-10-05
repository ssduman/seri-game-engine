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
		std::vector<Animation> animations{};

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

		void UpdateAnimations(const Animation& animation, double time, const Animation* previous, double previousTime, float weight)
		{
			for (const auto& mesh : meshes)
			{
				mesh->UpdateAnimation(animation, time, previous, previousTime, weight);
			}
		}

		const Animation* GetAnimation(int index) const
		{
			if (index < 0 || index >= static_cast<int>(animations.size()))
			{
				return nullptr;
			}

			return &animations[index];
		}

		int FindAnimation(const std::string& name) const
		{
			for (size_t i = 0; i < animations.size(); i++)
			{
				if (animations[i].name == name)
				{
					return static_cast<int>(i);
				}
			}

			return -1;
		}

	private:

	};
}
