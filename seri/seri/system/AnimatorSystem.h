#pragma once

#include <memory>
#include <string>
#include <cstdint>

namespace seri
{
	class Model;
	struct Animation;
}

namespace seri::component
{
	struct AnimatorComponent;
}

namespace seri::animation
{
	class AnimationStateMachine;
	struct AnimatorPlayback;
	struct AnimatorTransition;
	struct AnimatorCondition;
}

namespace seri::system
{
	class AnimatorSystem
	{
	public:
		static void Update();

		static const seri::Animation* GetClip(uint64_t clipAssetId, int clipIndex, const std::shared_ptr<seri::Model>& model);

		static const seri::Animation* GetCurrentClip(const seri::component::AnimatorComponent* animator, const std::shared_ptr<seri::Model>& model);

		static bool SetParameter(seri::component::AnimatorComponent& animator, const std::string& name, float value);

		static float GetParameter(const seri::component::AnimatorComponent& animator, const std::string& name);

		static std::string GetStateName(const seri::component::AnimatorComponent& animator);

		static void Play(seri::component::AnimatorComponent& animator, const std::string& name, float fade);

	private:
		static void UpdateClipMode(seri::component::AnimatorComponent& animator, const std::shared_ptr<seri::Model>& model);

		static void UpdateASM(seri::component::AnimatorComponent& animator, const seri::animation::AnimationStateMachine& asmAsset, const std::shared_ptr<seri::Model>& model);

		static void EnterState(seri::component::AnimatorComponent& animator, const seri::animation::AnimationStateMachine& asmAsset, const std::shared_ptr<seri::Model>& model, int stateIndex, float fade);

		static void StartClip(seri::component::AnimatorComponent& animator, uint64_t clipAssetId, int clipIndex, bool loop, float speed, float fade);

		static bool CanTransition(const seri::component::AnimatorComponent& animator, const seri::animation::AnimationStateMachine& asmAsset, const seri::animation::AnimatorTransition& transition);

		static bool CheckCondition(const seri::component::AnimatorComponent& animator, const seri::animation::AnimationStateMachine& asmAsset, const seri::animation::AnimatorCondition& condition);

		static float GetParameterValue(const seri::component::AnimatorComponent& animator, const seri::animation::AnimationStateMachine& asmAsset, const std::string& name);

		static void Advance(seri::component::AnimatorComponent& animator, const std::shared_ptr<seri::Model>& model, float deltaTime);

		static void AdvancePlayback(seri::animation::AnimatorPlayback& playback, const std::shared_ptr<seri::Model>& model, float step);

		static int FindClipIndex(uint64_t clipAssetId, const std::string& clipName, const std::shared_ptr<seri::Model>& model);

		static std::shared_ptr<seri::animation::AnimationStateMachine> GetASM(const seri::component::AnimatorComponent& animator);
	};
}
