#pragma once

#include "seri/util/Util.h"
#include "seri/scene/SceneManager.h"
#include "seri/component/Components.h"
#include "seri/graphic/Graphic.h"
#include "seri/asset/AssetManager.h"

#include <entt/entt.hpp>

#include <memory>
#include <vector>

namespace seri::system
{
	class MeshRendererSystem
	{
	public:
		static void Update()
		{
			auto& registry = seri::scene::SceneManager::GetRegistry();

			auto view = registry.view<
				seri::component::TransformComponent,
				seri::component::MeshComponent,
				seri::component::MeshRendererComponent
			>();

			for (entt::entity entity : view)
			{
				auto& transform = view.get<seri::component::TransformComponent>(entity);
				auto& mesh = view.get<seri::component::MeshComponent>(entity);
				auto& renderer = view.get<seri::component::MeshRendererComponent>(entity);

				if (mesh.meshAssetId == 0)
				{
					continue;
				}

				if (renderer.materialAssetId == 0)
				{
					continue;
				}

				std::shared_ptr<Model> model = seri::asset::AssetManager::GetAssetByID<Model>(mesh.meshAssetId);
				std::shared_ptr<Material> material = seri::asset::AssetManager::GetAssetByID<Material>(renderer.materialAssetId);

				seri::Graphic::DrawModel(
					model,
					material,
					transform.worldMatrix
				);
			}
		}
	};
}
