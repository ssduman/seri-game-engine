#pragma once

#include "gui/editor/GUIContext.h"

namespace seri::editor
{
	class ASMPanel
	{
	public:
		void Draw(GUIContext& ctx);

		void DrawInspector(GUIContext& ctx);

	private:
		void DrawToolbar(GUIContext& ctx, seri::animation::AnimationStateMachine& asmAsset, entt::entity previewEntity, bool live);

		void DrawParameters(seri::animation::AnimationStateMachine& asmAsset, seri::component::AnimatorComponent* animator);

		void DrawGraph(GUIContext& ctx, seri::animation::AnimationStateMachine& asmAsset, int liveState, float progress);

		void DrawNode(ImDrawList* drawList, const GUIContext& ctx, seri::animation::AnimationStateMachine& asmAsset, int node, const glm::vec2& origin, bool hovered, bool live, float progress);

		void DrawArrow(ImDrawList* drawList, const glm::vec2& start, const glm::vec2& end, ImU32 color);

		void DrawContextMenu(GUIContext& ctx, seri::animation::AnimationStateMachine& asmAsset);

		void DrawStateInspector(GUIContext& ctx, seri::animation::AnimationStateMachine& asmAsset);

		void DrawTransitionInspector(GUIContext& ctx, seri::animation::AnimationStateMachine& asmAsset);

		void DrawOverviewInspector(seri::animation::AnimationStateMachine& asmAsset);

		bool DrawCondition(seri::animation::AnimationStateMachine& asmAsset, seri::animation::AnimatorCondition& condition, bool& remove);

		bool DrawNameInput(const char* label, const std::string& name, std::string& result);

		void Select(GUIContext& ctx, int node, int transition);

		void ValidateSelection(GUIContext& ctx, const seri::animation::AnimationStateMachine& asmAsset);

		void FrameAll(seri::animation::AnimationStateMachine& asmAsset);

		void CreateState(GUIContext& ctx, seri::animation::AnimationStateMachine& asmAsset, const glm::vec2& position);

		void DeleteState(GUIContext& ctx, seri::animation::AnimationStateMachine& asmAsset, int stateIndex);

		void RenameState(seri::animation::AnimationStateMachine& asmAsset, int stateIndex, const std::string& name);

		void AddTransition(GUIContext& ctx, seri::animation::AnimationStateMachine& asmAsset, int from, int to);

		void DeleteTransition(GUIContext& ctx, seri::animation::AnimationStateMachine& asmAsset, int transitionIndex);

		void AddParameter(seri::animation::AnimationStateMachine& asmAsset, seri::animation::AnimatorParameterType type);

		void RenameParameter(seri::animation::AnimationStateMachine& asmAsset, int parameterIndex, const std::string& name);

		void DeleteParameter(seri::animation::AnimationStateMachine& asmAsset, int parameterIndex);

		entt::entity FindPreviewEntity(const GUIContext& ctx);

		std::shared_ptr<seri::Model> FindClipModel(const GUIContext& ctx, const seri::animation::AnimatorState& state);

		int HitNode(seri::animation::AnimationStateMachine& asmAsset, const glm::vec2& point);

		int HitTransition(seri::animation::AnimationStateMachine& asmAsset, const glm::vec2& point);

		bool GetTransitionSegment(seri::animation::AnimationStateMachine& asmAsset, const seri::animation::AnimatorTransition& transition, glm::vec2& start, glm::vec2& end);

		glm::vec2& GetNodePosition(seri::animation::AnimationStateMachine& asmAsset, int node);

		glm::vec2 GetNodeSize(seri::animation::AnimationStateMachine& asmAsset, int node);

		std::string GetNodeName(seri::animation::AnimationStateMachine& asmAsset, int node);

		static ImVec2 ToImVec2(const glm::vec2& v);

		static glm::vec2 ToVec2(const ImVec2& v);

		static constexpr int kNoNode = -1;
		static constexpr int kAnyNode = -2;
		static constexpr float kNodeMinWidth = 120.0f;
		static constexpr float kNodeHeight = 40.0f;
		static constexpr float kTransitionOffset = 7.0f;
		static constexpr float kTransitionHitDistance = 6.0f;
		static constexpr float kGridStep = 32.0f;

		glm::vec2 _pan{ 0.0f, 0.0f };
		uint64_t _framedAssetId{ 0 };
		int _dragNode{ kNoNode };
		int _linkFrom{ kNoNode };
		int _contextNode{ kNoNode };
		int _contextTransition{ -1 };
		glm::vec2 _contextPosition{ 0.0f, 0.0f };

		ImGuiID _nameEditId{ 0 };
		std::string _nameEditBuffer{};
		std::string _nameScratch{};

	};
}
