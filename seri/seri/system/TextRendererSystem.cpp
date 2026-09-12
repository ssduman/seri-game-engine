#include "Seripch.h"
#include "seri/system/TextRendererSystem.h"

#include "seri/scene/SceneManager.h"
#include "seri/component/Components.h"
#include "seri/graphic/Graphic.h"
#include "seri/graphic/Mesh.h"
#include "seri/graphic/Material.h"
#include "seri/shader/ShaderLibrary.h"
#include "seri/font/Font.h"
#include "seri/font/FontManager.h"
#include "seri/font/TextMesh.h"
#include "seri/system/UISystem.h"
#include <entt/entt.hpp>
#include <memory>

namespace seri::system
{
	void TextRendererSystem::Update()
	{
		auto& registry = seri::scene::SceneManager::GetRegistry();

		auto view = registry.view<
			seri::component::TransformComponent,
			seri::component::TextComponent
		>();

		for (entt::entity entity : view)
		{
			auto& transform = view.get<seri::component::TransformComponent>(entity);
			auto& text = view.get<seri::component::TextComponent>(entity);

			if (!transform.isActiveInHierarchy)
			{
				continue;
			}

			std::shared_ptr<seri::font::Font> font = seri::font::FontManager::GetFont(text.fontAssetId);
			if (!font)
			{
				continue;
			}

			if (!text.material)
			{
				text.material = std::make_shared<Material>();
				text.material->SetShader(ShaderLibrary::Find("text"));
			}

			seri::component::TextComponent::BuiltState wanted{
				font->id,
				text.text,
				text.fontSize,
				text.lineSpacing,
				text.alignH,
				text.alignV
			};

			if (!text.mesh || !(text.built == wanted))
			{
				if (!text.mesh)
				{
					text.mesh = std::make_shared<Mesh>();
				}

				seri::font::TextDesc desc{};
				desc.fontSize = text.fontSize;
				desc.lineSpacing = text.lineSpacing;
				desc.alignH = text.alignH;
				desc.alignV = text.alignV;

				seri::font::TextMesh::BuildTextMesh(*text.mesh, *font, text.text, desc);

				text.built = wanted;
			}

			if (!text.mesh->GetVao())
			{
				continue;
			}

			text.material->SetTexture(literals::kUniformTextTexture, font->GetAtlas());
			text.material->SetFloat4(literals::kUniformTextColor, text.color);

			bool isUI = UISystem::IsScreenSpace(entity);

			seri::Graphic::Draw(
				text.mesh,
				text.material,
				transform.worldMatrix,
				isUI ? PassType::ui : PassType::transparent,
				isUI ? UISystem::GetDrawOrder(entity) : 0
			);
		}
	}
}
