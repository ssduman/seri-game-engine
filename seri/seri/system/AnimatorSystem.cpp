#include "Seripch.h"
#include "seri/system/AnimatorSystem.h"

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
	void AnimatorSystem::Update()
	{
		SERI_PROFILER_ZONE_SCOPED;

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

			if (animator.activeClipAssetId != animator.clipAssetId || animator.activeClipIndex != animator.clipIndex)
			{
				if (animator.activeClipIndex >= 0)
				{
					animator.previousClipAssetId = animator.activeClipAssetId;
					animator.previousClipIndex = animator.fadeDuration > 0.0f ? animator.activeClipIndex : -1;
					animator.previousTime = animator.time;
					animator.fadeTime = 0.0f;
					animator.time = 0.0f;
				}

				animator.activeClipAssetId = animator.clipAssetId;
				animator.activeClipIndex = animator.clipIndex;
			}

			const Animation* clip = GetClip(animator.clipAssetId, animator.clipIndex, model);
			if (!clip)
			{
				continue;
			}

			float step = animator.playing ? deltaTime * animator.speed : 0.0f;

			animator.time = WrapTime(animator.time + step, clip->GetDuration(), animator.loop);

			if (animator.previousClipIndex >= 0)
			{
				const Animation* previous = GetClip(animator.previousClipAssetId, animator.previousClipIndex, model);

				animator.fadeTime += animator.playing ? deltaTime : 0.0f;

				if (!previous || animator.fadeTime >= animator.fadeDuration)
				{
					animator.previousClipIndex = -1;
				}
				else
				{
					animator.previousTime = WrapTime(animator.previousTime + step, previous->GetDuration(), animator.loop);
				}
			}
		}
	}

	const Animation* AnimatorSystem::GetClip(uint64_t clipAssetId, int clipIndex, const std::shared_ptr<Model>& model)
	{
		if (clipAssetId == 0)
		{
			return model ? model->GetAnimation(clipIndex) : nullptr;
		}

		std::shared_ptr<Model> source = seri::asset::AssetManager::GetAssetByID<Model>(clipAssetId);

		return source ? source->GetAnimation(clipIndex) : nullptr;
	}

	float AnimatorSystem::WrapTime(float time, double duration, bool loop)
	{
		if (duration <= 0.0)
		{
			return 0.0f;
		}

		if (!loop)
		{
			return glm::clamp(time, 0.0f, static_cast<float>(duration));
		}

		float wrapped = static_cast<float>(std::fmod(time, duration));
		if (wrapped < 0.0f)
		{
			wrapped += static_cast<float>(duration);
		}

		return wrapped;
	}
}
