#pragma once

#include "seri/util/Util.h"
#include "seri/scene/SceneManager.h"
#include "seri/component/Components.h"
#include "seri/graphic/Graphic.h"
#include "seri/graphic/Model.h"
#include "seri/graphic/Material.h"
#include "seri/asset/AssetManager.h"

#include <entt/entt.hpp>

#include <memory>

namespace seri::system
{
	class SkinnedMeshRendererSystem
	{
	public:
		static void Update()
		{
			auto& registry = seri::scene::SceneManager::GetRegistry();

			auto view = registry.view<
				seri::component::TransformComponent,
				seri::component::SkinnedMeshRendererComponent
			>();

			for (entt::entity entity : view)
			{
				auto& transform = view.get<seri::component::TransformComponent>(entity);
				auto& renderer = view.get<seri::component::SkinnedMeshRendererComponent>(entity);

				if (renderer.meshAssetId == 0)
				{
					continue;
				}

				if (renderer.materialAssetIds.empty())
				{
					continue;
				}

				std::shared_ptr<Model> model = seri::asset::AssetManager::GetAssetByID<Model>(renderer.meshAssetId);

				if (!model)
				{
					continue;
				}

				if (model->GetAnimationDuration() > 0.0)
				{
					auto* animator = registry.try_get<seri::component::AnimatorComponent>(entity);
					model->UpdateAnimations(animator ? animator->time : 0.0f);
				}

				int slotCount = static_cast<int>(renderer.materialAssetIds.size());

				for (const auto& mesh : model->meshes)
				{
					int slot = mesh->materialIndex;
					if (slot < 0 || slot >= slotCount)
					{
						slot = slotCount - 1;
					}

					std::shared_ptr<Material> material = seri::asset::AssetManager::GetAssetByID<Material>(renderer.materialAssetIds[slot]);

					if (!material)
					{
						continue;
					}

					seri::Graphic::Draw(mesh, material, transform.worldMatrix);
				}
			}
		}
	};
}
