#pragma once

#include "seri/util/Util.h"
#include "seri/core/TimeWrapper.h"
#include "seri/scene/SceneManager.h"
#include "seri/component/Components.h"
#include "seri/graphic/Model.h"
#include "seri/asset/AssetManager.h"

#include <entt/entt.hpp>

#include <cmath>
#include <memory>

namespace seri::system
{
	class AnimatorSystem
	{
	public:
		static void Update()
		{
			auto& registry = seri::scene::SceneManager::GetRegistry();

			auto view = registry.view<
				seri::component::AnimatorComponent,
				seri::component::SkinnedMeshRendererComponent
			>();

			float deltaTime = TimeWrapper::GetDeltaTime();

			for (entt::entity entity : view)
			{
				auto& animator = view.get<seri::component::AnimatorComponent>(entity);
				auto& renderer = view.get<seri::component::SkinnedMeshRendererComponent>(entity);

				if (renderer.meshAssetId == 0)
				{
					continue;
				}

				std::shared_ptr<Model> model = seri::asset::AssetManager::GetAssetByID<Model>(renderer.meshAssetId);

				if (!model)
				{
					continue;
				}

				double duration = model->GetAnimationDuration();

				if (duration <= 0.0)
				{
					continue;
				}

				if (animator.playing)
				{
					animator.time += deltaTime * animator.speed;
				}

				if (animator.loop)
				{
					animator.time = static_cast<float>(std::fmod(animator.time, duration));
					if (animator.time < 0.0f)
					{
						animator.time += static_cast<float>(duration);
					}
				}
				else
				{
					animator.time = glm::clamp(animator.time, 0.0f, static_cast<float>(duration));
				}

				model->UpdateAnimations(animator.time);
			}
		}
	};
}
