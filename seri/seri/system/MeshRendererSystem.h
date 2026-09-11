#pragma once

#include "seri/util/Util.h"
#include "seri/scene/SceneManager.h"
#include "seri/component/Components.h"
#include "seri/graphic/Graphic.h"
#include "seri/graphic/Model.h"
#include "seri/graphic/Material.h"
#include "seri/shader/ShaderLibrary.h"
#include "seri/asset/AssetManager.h"
#include "seri/rendering/render/RenderingManager.h"

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
			static std::shared_ptr<Material> shadowMaterial = []()
				{
					auto mat = std::make_shared<Material>();
					mat->SetShader(ShaderLibrary::Find("shadow"));
					return mat;
				}();

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

				if (!transform.isActiveInHierarchy)
				{
					continue;
				}

				if (mesh.meshAssetId == 0)
				{
					continue;
				}

				if (renderer.materialAssetIds.empty())
				{
					continue;
				}

				std::shared_ptr<Model> model = seri::asset::AssetManager::GetAssetByID<Model>(mesh.meshAssetId);

				if (!model)
				{
					continue;
				}

				int slotCount = static_cast<int>(renderer.materialAssetIds.size());

				for (const auto& meshPart : model->meshes)
				{
					int slot = meshPart->materialIndex;
					if (slot < 0 || slot >= slotCount)
					{
						slot = slotCount - 1;
					}

					std::shared_ptr<Material> material = seri::asset::AssetManager::GetAssetByID<Material>(renderer.materialAssetIds[slot]);

					if (!material)
					{
						continue;
					}

					seri::Graphic::Draw(meshPart, material, transform.worldMatrix);
				}

				if (renderer.castShadow)
				{
					for (const auto& meshPart : model->meshes)
					{
						RenderItem shadowCmd{};
						shadowCmd.type = PassType::shadow;
						shadowCmd.name = "shadow";
						shadowCmd.material = shadowMaterial;
						shadowCmd.model = transform.worldMatrix * meshPart->transformation;
						shadowCmd.vao = meshPart->GetVao();
						seri::RenderingManager::Submit(shadowCmd);
					}
				}
			}
		}
	};
}
