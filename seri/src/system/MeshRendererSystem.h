#pragma once

#include "util/Util.h"
#include "scene/SceneManager.h"
#include "component/Components.h"
#include "graphic/Graphic.h"
#include "graphic/Model.h"
#include "graphic/Material.h"
#include "shader/ShaderLibrary.h"
#include "asset/AssetManager.h"
#include "rendering/render/RenderingManager.h"

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

				if (!model || !material)
				{
					continue;
				}

				seri::Graphic::DrawModel(
					model,
					material,
					transform.worldMatrix
				);

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
