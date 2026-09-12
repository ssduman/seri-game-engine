#include "Seripch.h"
#include "seri/system/SpriteRendererSystem.h"

#include "seri/scene/SceneManager.h"
#include "seri/component/Components.h"
#include "seri/graphic/Graphic.h"
#include "seri/graphic/Mesh.h"
#include "seri/graphic/Material.h"
#include "seri/shader/ShaderLibrary.h"
#include "seri/texture/TextureBase.h"
#include "seri/asset/AssetManager.h"
#include "seri/system/UISystem.h"
#include <entt/entt.hpp>
#include <memory>

namespace seri::system
{
	void SpriteRendererSystem::Update()
	{
		auto& registry = seri::scene::SceneManager::GetRegistry();

		auto view = registry.view<
			seri::component::TransformComponent,
			seri::component::SpriteRendererComponent
		>();

		for (entt::entity entity : view)
		{
			auto& transform = view.get<seri::component::TransformComponent>(entity);
			auto& sprite = view.get<seri::component::SpriteRendererComponent>(entity);

			if (!transform.isActiveInHierarchy)
			{
				continue;
			}

			std::shared_ptr<seri::TextureBase> texture = seri::asset::AssetManager::GetAssetByID<seri::TextureBase>(sprite.textureAssetId);
			if (!texture)
			{
				continue;
			}

			if (!sprite.material)
			{
				sprite.material = std::make_shared<Material>();
				sprite.material->SetShader(ShaderLibrary::Find("sprite"));
			}

			glm::vec2 size{ 1.0f, 1.0f };
			glm::vec2 pivot{ 0.5f, 0.5f };

			if (auto* rect = registry.try_get<seri::component::RectComponent>(entity))
			{
				size = rect->resolvedSize;
				pivot = rect->pivot;
			}

			seri::component::SpriteRendererComponent::BuiltState wanted{
				sprite.flipX,
				sprite.flipY,
				size,
				pivot
			};

			if (!sprite.mesh || !(sprite.built == wanted))
			{
				if (!sprite.mesh)
				{
					sprite.mesh = std::make_shared<Mesh>();
				}

				BuildQuad(*sprite.mesh, size, pivot, sprite.flipX, sprite.flipY);

				sprite.built = wanted;
			}

			if (!sprite.mesh->GetVao())
			{
				continue;
			}

			sprite.material->SetTexture(literals::kUniformSpriteTexture, texture);
			sprite.material->SetFloat4(literals::kUniformSpriteColor, sprite.color);

			bool isUI = UISystem::IsScreenSpace(entity);

			seri::Graphic::Draw(
				sprite.mesh,
				sprite.material,
				transform.worldMatrix,
				isUI ? PassType::ui : PassType::transparent,
				isUI ? UISystem::GetDrawOrder(entity) : 0
			);
		}
	}

	void SpriteRendererSystem::BuildQuad(Mesh& mesh, const glm::vec2& size, const glm::vec2& pivot, bool flipX, bool flipY)
	{
		mesh.Clear();

		float u0 = flipX ? 1.0f : 0.0f;
		float u1 = flipX ? 0.0f : 1.0f;
		float v0 = flipY ? 1.0f : 0.0f;
		float v1 = flipY ? 0.0f : 1.0f;

		float x0 = -pivot.x * size.x;
		float x1 = (1.0f - pivot.x) * size.x;
		float y0 = -pivot.y * size.y;
		float y1 = (1.0f - pivot.y) * size.y;

		mesh.vertices = {
			{ x0, y0, 0.0f },
			{ x1, y0, 0.0f },
			{ x1, y1, 0.0f },
			{ x0, y1, 0.0f },
		};

		mesh.uv0s = {
			{ u0, v0 },
			{ u1, v0 },
			{ u1, v1 },
			{ u0, v1 },
		};

		mesh.indices = { 0, 1, 2, 2, 3, 0 };

		mesh.Update();
	}
}
