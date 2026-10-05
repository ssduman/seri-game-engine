#include "Seripch.h"
#include "seri/system/AnimatorSystem.h"

#include "seri/util/Util.h"
#include "seri/core/TimeWrapper.h"
#include "seri/scene/SceneManager.h"
#include "seri/component/Components.h"
#include "seri/graphic/Model.h"
#include "seri/animation/AnimationStateMachine.h"
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

			if (animator.activeAsmAssetId != animator.asmAssetId)
			{
				if (animator.activeAsmAssetId != 0)
				{
					animator.parameters.clear();
				}

				animator.activeAsmAssetId = animator.asmAssetId;
				animator.stateIndex = -1;
			}

			if (animator.mode == seri::animation::AnimatorMode::animation_state_machine)
			{
				std::shared_ptr<seri::animation::AnimationStateMachine> asmAsset = GetASM(animator);

				if (!asmAsset || asmAsset->states.empty())
				{
					animator.current.clipIndex = -1;
					animator.previous.clipIndex = -1;
					animator.stateIndex = -1;
					continue;
				}

				UpdateASM(animator, *asmAsset, model);
			}
			else
			{
				animator.stateIndex = -1;
				UpdateClipMode(animator, model);
			}

			Advance(animator, model, deltaTime);
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

	const Animation* AnimatorSystem::GetCurrentClip(const seri::component::AnimatorComponent* animator, const std::shared_ptr<Model>& model)
	{
		if (!animator)
		{
			return model ? model->GetAnimation(0) : nullptr;
		}

		if (animator->current.clipIndex >= 0)
		{
			return GetClip(animator->current.clipAssetId, animator->current.clipIndex, model);
		}

		if (animator->mode == seri::animation::AnimatorMode::clip)
		{
			return GetClip(animator->clipAssetId, animator->clipIndex, model);
		}

		std::shared_ptr<seri::animation::AnimationStateMachine> asmAsset = GetASM(*animator);

		if (!asmAsset || asmAsset->states.empty())
		{
			return nullptr;
		}

		int entry = asmAsset->FindState(asmAsset->entryState);
		const seri::animation::AnimatorState& state = asmAsset->states[entry >= 0 ? entry : 0];
		uint64_t clipAssetId = state.clipAssetId != 0 ? state.clipAssetId : animator->clipAssetId;

		return GetClip(clipAssetId, FindClipIndex(clipAssetId, state.clip, model), model);
	}

	bool AnimatorSystem::SetParameter(seri::component::AnimatorComponent& animator, const std::string& name, float value)
	{
		std::shared_ptr<seri::animation::AnimationStateMachine> asmAsset = GetASM(animator);

		if (!asmAsset || !asmAsset->FindParameter(name))
		{
			LIB_LOGGER(warning, animator) << "parameter '" << name << "' not found";
			return false;
		}

		animator.parameters[name] = value;

		return true;
	}

	float AnimatorSystem::GetParameter(const seri::component::AnimatorComponent& animator, const std::string& name)
	{
		std::shared_ptr<seri::animation::AnimationStateMachine> asmAsset = GetASM(animator);

		return asmAsset ? GetParameterValue(animator, *asmAsset, name) : 0.0f;
	}

	std::string AnimatorSystem::GetStateName(const seri::component::AnimatorComponent& animator)
	{
		std::shared_ptr<seri::animation::AnimationStateMachine> asmAsset = GetASM(animator);

		if (!asmAsset || animator.stateIndex < 0 || animator.stateIndex >= static_cast<int>(asmAsset->states.size()))
		{
			return "";
		}

		return asmAsset->states[animator.stateIndex].name;
	}

	void AnimatorSystem::Play(seri::component::AnimatorComponent& animator, const std::string& name, float fade)
	{
		animator.requestedPlay = name;
		animator.requestedFade = fade;
	}

	void AnimatorSystem::UpdateClipMode(seri::component::AnimatorComponent& animator, const std::shared_ptr<Model>& model)
	{
		float fade = animator.fadeDuration;

		if (!animator.requestedPlay.empty())
		{
			int clipIndex = FindClipIndex(animator.clipAssetId, animator.requestedPlay, model);
			if (clipIndex >= 0)
			{
				animator.clipIndex = clipIndex;
				fade = animator.requestedFade;
			}
			else
			{
				LIB_LOGGER(warning, animator) << "clip '" << animator.requestedPlay << "' not found";
			}

			animator.requestedPlay.clear();
		}

		animator.current.loop = animator.loop;

		if (animator.current.clipAssetId != animator.clipAssetId || animator.current.clipIndex != animator.clipIndex)
		{
			StartClip(animator, animator.clipAssetId, animator.clipIndex, animator.loop, 1.0f, fade);
		}
	}

	void AnimatorSystem::UpdateASM(seri::component::AnimatorComponent& animator, const seri::animation::AnimationStateMachine& asmAsset, const std::shared_ptr<Model>& model)
	{
		if (animator.stateIndex < 0 || animator.stateIndex >= static_cast<int>(asmAsset.states.size()))
		{
			int entry = asmAsset.FindState(asmAsset.entryState);
			EnterState(animator, asmAsset, model, entry >= 0 ? entry : 0, 0.0f);
		}

		if (!animator.requestedPlay.empty())
		{
			int stateIndex = asmAsset.FindState(animator.requestedPlay);
			if (stateIndex >= 0)
			{
				EnterState(animator, asmAsset, model, stateIndex, animator.requestedFade);
			}
			else
			{
				LIB_LOGGER(warning, animator) << "state '" << animator.requestedPlay << "' not found";
			}

			animator.requestedPlay.clear();
			return;
		}

		const std::string& stateName = asmAsset.states[animator.stateIndex].name;

		for (int pass = 0; pass < 2; pass++)
		{
			for (const auto& transition : asmAsset.transitions)
			{
				bool fromMatches = pass == 0
					? transition.from == seri::animation::AnimationStateMachine::kAnyState && transition.to != stateName
					: transition.from == stateName;

				if (!fromMatches || !CanTransition(animator, asmAsset, transition))
				{
					continue;
				}

				int target = asmAsset.FindState(transition.to);
				if (target < 0)
				{
					LIB_LOGGER(warning, animator) << "transition target '" << transition.to << "' not found";
					continue;
				}

				for (const auto& condition : transition.conditions)
				{
					const seri::animation::AnimatorParameter* parameter = asmAsset.FindParameter(condition.parameter);
					if (parameter && parameter->type == seri::animation::AnimatorParameterType::trigger)
					{
						animator.parameters[condition.parameter] = 0.0f;
					}
				}

				EnterState(animator, asmAsset, model, target, transition.fade);
				return;
			}
		}
	}

	void AnimatorSystem::EnterState(seri::component::AnimatorComponent& animator, const seri::animation::AnimationStateMachine& asmAsset, const std::shared_ptr<Model>& model, int stateIndex, float fade)
	{
		animator.stateIndex = stateIndex;

		const seri::animation::AnimatorState& state = asmAsset.states[stateIndex];
		uint64_t clipAssetId = state.clipAssetId != 0 ? state.clipAssetId : animator.clipAssetId;

		int clipIndex = FindClipIndex(clipAssetId, state.clip, model);
		if (clipIndex < 0)
		{
			LIB_LOGGER(warning, animator) << "clip '" << state.clip << "' of state '" << state.name << "' not found";
		}

		StartClip(animator, clipAssetId, clipIndex, state.loop, state.speed, fade);

		const Animation* clip = GetClip(clipAssetId, clipIndex, model);
		if (clip && state.speed * animator.speed < 0.0f)
		{
			animator.current.time = static_cast<float>(clip->GetDuration());
		}
	}

	void AnimatorSystem::StartClip(seri::component::AnimatorComponent& animator, uint64_t clipAssetId, int clipIndex, bool loop, float speed, float fade)
	{
		if (animator.current.clipIndex >= 0 && fade > 0.0f)
		{
			animator.previous = animator.current;
			animator.fadeTime = 0.0f;
			animator.fadeLength = fade;
		}
		else
		{
			animator.previous.clipIndex = -1;
		}

		animator.current = seri::animation::AnimatorPlayback{
			.clipAssetId = clipAssetId,
			.clipIndex = clipIndex,
			.loop = loop,
			.finished = false,
			.speed = speed,
			.time = 0.0f
		};
	}

	bool AnimatorSystem::CanTransition(const seri::component::AnimatorComponent& animator, const seri::animation::AnimationStateMachine& asmAsset, const seri::animation::AnimatorTransition& transition)
	{
		if (transition.exitTime && !animator.current.finished)
		{
			return false;
		}

		for (const auto& condition : transition.conditions)
		{
			if (!CheckCondition(animator, asmAsset, condition))
			{
				return false;
			}
		}

		return true;
	}

	bool AnimatorSystem::CheckCondition(const seri::component::AnimatorComponent& animator, const seri::animation::AnimationStateMachine& asmAsset, const seri::animation::AnimatorCondition& condition)
	{
		const seri::animation::AnimatorParameter* parameter = asmAsset.FindParameter(condition.parameter);
		if (!parameter)
		{
			return false;
		}

		float value = GetParameterValue(animator, asmAsset, condition.parameter);

		if (parameter->type == seri::animation::AnimatorParameterType::trigger)
		{
			return value != 0.0f;
		}

		switch (condition.op)
		{
			case seri::animation::AnimatorConditionOp::greater: return value > condition.value;
			case seri::animation::AnimatorConditionOp::less: return value < condition.value;
			case seri::animation::AnimatorConditionOp::equals: return value == condition.value;
			case seri::animation::AnimatorConditionOp::not_equals: return value != condition.value;
		}

		return false;
	}

	float AnimatorSystem::GetParameterValue(const seri::component::AnimatorComponent& animator, const seri::animation::AnimationStateMachine& asmAsset, const std::string& name)
	{
		auto it = animator.parameters.find(name);
		if (it != animator.parameters.end())
		{
			return it->second;
		}

		const seri::animation::AnimatorParameter* parameter = asmAsset.FindParameter(name);

		return parameter ? parameter->value : 0.0f;
	}

	void AnimatorSystem::Advance(seri::component::AnimatorComponent& animator, const std::shared_ptr<Model>& model, float deltaTime)
	{
		float step = animator.playing ? deltaTime * animator.speed : 0.0f;

		AdvancePlayback(animator.current, model, step * animator.current.speed);

		if (animator.previous.clipIndex < 0)
		{
			return;
		}

		animator.fadeTime += animator.playing ? deltaTime : 0.0f;

		if (animator.fadeTime >= animator.fadeLength || !GetClip(animator.previous.clipAssetId, animator.previous.clipIndex, model))
		{
			animator.previous.clipIndex = -1;
			return;
		}

		AdvancePlayback(animator.previous, model, step * animator.previous.speed);
	}

	void AnimatorSystem::AdvancePlayback(seri::animation::AnimatorPlayback& playback, const std::shared_ptr<Model>& model, float step)
	{
		const Animation* clip = GetClip(playback.clipAssetId, playback.clipIndex, model);
		if (!clip)
		{
			return;
		}

		float duration = static_cast<float>(clip->GetDuration());
		if (duration <= 0.0f)
		{
			return;
		}

		float time = playback.time + step;

		if (!playback.loop)
		{
			playback.time = glm::clamp(time, 0.0f, duration);
			playback.finished = playback.finished || (step >= 0.0f ? playback.time >= duration : playback.time <= 0.0f);
			return;
		}

		if (time >= duration || time < 0.0f)
		{
			playback.finished = true;
		}

		playback.time = std::fmod(time, duration);
		if (playback.time < 0.0f)
		{
			playback.time += duration;
		}
	}

	int AnimatorSystem::FindClipIndex(uint64_t clipAssetId, const std::string& clipName, const std::shared_ptr<Model>& model)
	{
		std::shared_ptr<Model> source = clipAssetId != 0 ? seri::asset::AssetManager::GetAssetByID<Model>(clipAssetId) : model;

		return source ? source->FindAnimation(clipName) : -1;
	}

	std::shared_ptr<seri::animation::AnimationStateMachine> AnimatorSystem::GetASM(const seri::component::AnimatorComponent& animator)
	{
		if (animator.asmAssetId == 0)
		{
			return nullptr;
		}

		return seri::asset::AssetManager::GetAssetByID<seri::animation::AnimationStateMachine>(animator.asmAssetId);
	}
}
