#include "Editorpch.h"

#include <seri/system/AnimatorSystem.h>

#include "gui/editor/panels/ASMPanel.h"
#include "gui/common/GUIWidgets.h"

#include <cmath>
#include <algorithm>

namespace seri::editor
{
	void ASMPanel::Draw(GUIContext& ctx)
	{
		auto asmAsset = seri::asset::AssetManager::GetAssetByID<seri::animation::AnimationStateMachine>(ctx.asmAssetId);
		if (!asmAsset)
		{
			ImGui::TextDisabled("double click an animation state machine asset in the project panel to edit it");
			return;
		}

		if (_framedAssetId != ctx.asmAssetId)
		{
			_framedAssetId = ctx.asmAssetId;
			_dragNode = kNoNode;
			_linkFrom = kNoNode;
			ctx.asmNodeIndex = kNoNode;
			ctx.asmTransitionIndex = -1;
			FrameAll(*asmAsset);
		}

		ValidateSelection(ctx, *asmAsset);

		auto& registry = seri::scene::SceneManager::GetRegistry();

		entt::entity previewEntity = FindPreviewEntity(ctx);
		seri::component::AnimatorComponent* animator = previewEntity != entt::null ? registry.try_get<seri::component::AnimatorComponent>(previewEntity) : nullptr;

		bool live = animator &&
			seri::scene::SceneManager::GetState() != seri::scene::SceneState::edit &&
			animator->mode == seri::animation::AnimatorMode::animation_state_machine &&
			animator->activeAsmAssetId == ctx.asmAssetId;

		int liveState = kNoNode;
		float progress = 0.0f;

		if (live && animator->stateIndex >= 0 && animator->stateIndex < static_cast<int>(asmAsset->states.size()))
		{
			liveState = animator->stateIndex;

			auto* renderer = registry.try_get<seri::component::SkinnedMeshRendererComponent>(previewEntity);
			std::shared_ptr<seri::Model> model = renderer ? seri::asset::AssetManager::GetAssetByID<seri::Model>(renderer->meshAssetId) : nullptr;

			const seri::Animation* clip = seri::system::AnimatorSystem::GetCurrentClip(animator, model);
			float duration = clip ? static_cast<float>(clip->GetDuration()) : 0.0f;
			if (duration > 0.0f)
			{
				progress = animator->current.loop
					? std::fmod(animator->current.time, duration) / duration
					: animator->current.time / duration;
				progress = std::clamp(progress < 0.0f ? progress + 1.0f : progress, 0.0f, 1.0f);
			}
		}

		DrawToolbar(ctx, *asmAsset, previewEntity, live);

		ImGui::BeginChild("##Parameters", ImVec2(260.0f, 0.0f), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX);
		DrawParameters(*asmAsset, live ? animator : nullptr);
		ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginChild("##Graph", ImVec2(0.0f, 0.0f), ImGuiChildFlags_Borders, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoMove);
		DrawGraph(ctx, *asmAsset, liveState, progress);
		ImGui::EndChild();
	}

	void ASMPanel::DrawInspector(GUIContext& ctx)
	{
		auto asmAsset = seri::asset::AssetManager::GetAssetByID<seri::animation::AnimationStateMachine>(ctx.asmAssetId);
		if (!asmAsset)
		{
			ImGui::TextDisabled("animation state machine not loaded");
			return;
		}

		ValidateSelection(ctx, *asmAsset);

		{
			ScopedChild scopedChild("##ASMHeader");

			std::string name = seri::asset::AssetManager::GetAssetName(ctx.asmAssetId);
			ImGui::TextUnformatted(fmt::format("{}{}", name, asmAsset->dirty ? "*" : "").c_str());
			ImGui::SameLine();
			ImGui::TextDisabled("(%s)", seri::asset::AssetTypeToString(seri::asset::AssetType::animation_state_machine));
		}

		if (ctx.asmNodeIndex >= 0)
		{
			DrawStateInspector(ctx, *asmAsset);
		}
		else if (ctx.asmNodeIndex == kAnyNode)
		{
			ScopedChild scopedChild("##ASMAnyState");

			ImGui::TextUnformatted("Any State");
			ImGui::Separator();
			ImGui::TextWrapped("Transitions from Any State are checked before the current state's and can start from every state except their target.");
		}
		else if (ctx.asmTransitionIndex >= 0)
		{
			DrawTransitionInspector(ctx, *asmAsset);
		}
		else
		{
			DrawOverviewInspector(*asmAsset);
		}
	}

	void ASMPanel::DrawToolbar(GUIContext& ctx, seri::animation::AnimationStateMachine& asmAsset, entt::entity previewEntity, bool live)
	{
		std::string name = seri::asset::AssetManager::GetAssetName(ctx.asmAssetId);

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(fmt::format("{}{}", name, asmAsset.dirty ? "*" : "").c_str());

		ImGui::SameLine();
		if (ImGui::Button("Save"))
		{
			seri::asset::AssetManager::SaveAsset(ctx.asmAssetId);
		}

		ImGui::SameLine();
		if (ImGui::Button("Frame"))
		{
			FrameAll(asmAsset);
		}

		ImGui::SameLine();

		if (_linkFrom != kNoNode)
		{
			ImGui::TextDisabled("click the target state, right click to cancel");
		}
		else if (live)
		{
			auto* idComp = seri::scene::SceneManager::GetRegistry().try_get<seri::component::IDComponent>(previewEntity);
			ImGui::TextDisabled("live: %s", idComp ? idComp->name.c_str() : "entity");
		}
		else
		{
			ImGui::TextDisabled("right click to add states and transitions, drag with right or middle mouse to pan");
		}
	}

	void ASMPanel::DrawParameters(seri::animation::AnimationStateMachine& asmAsset, seri::component::AnimatorComponent* animator)
	{
		float buttonWidth = ImGui::GetFrameHeight();
		float spacing = ImGui::GetStyle().ItemSpacing.x;
		float valueWidth = 64.0f;

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted("Parameters");

		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - buttonWidth);

		if (ImGui::Button("+", ImVec2(buttonWidth, 0.0f)))
		{
			ImGui::OpenPopup("##AddParameter");
		}

		if (ImGui::BeginPopup("##AddParameter"))
		{
			for (auto type : { seri::animation::AnimatorParameterType::number, seri::animation::AnimatorParameterType::boolean, seri::animation::AnimatorParameterType::trigger })
			{
				if (ImGui::MenuItem(seri::animation::AnimatorParameterTypeToString(type)))
				{
					AddParameter(asmAsset, type);
				}
			}

			ImGui::EndPopup();
		}

		ImGui::Separator();

		if (asmAsset.parameters.empty())
		{
			ImGui::TextDisabled("no parameters");
			return;
		}

		int removeIndex = -1;

		for (int i = 0; i < static_cast<int>(asmAsset.parameters.size()); i++)
		{
			seri::animation::AnimatorParameter& parameter = asmAsset.parameters[i];

			ImGui::PushID(i);

			float rowStart = ImGui::GetCursorPosX();
			float rowWidth = ImGui::GetContentRegionAvail().x;
			float nameWidth = rowWidth - valueWidth - buttonWidth - spacing * 2.0f;

			ImGui::SetNextItemWidth(nameWidth);

			std::string name;
			if (DrawNameInput("##name", parameter.name, name))
			{
				RenameParameter(asmAsset, i, name);
			}

			if (ImGui::IsItemHovered())
			{
				ImGui::SetTooltip("%s", seri::animation::AnimatorParameterTypeToString(parameter.type));
			}

			float value = animator ? seri::system::AnimatorSystem::GetParameter(*animator, parameter.name) : parameter.value;
			bool changed = false;

			ImGui::SameLine(rowStart + nameWidth + spacing);
			ImGui::SetNextItemWidth(valueWidth);

			switch (parameter.type)
			{
				case seri::animation::AnimatorParameterType::number:
					{
						changed = ImGui::DragFloat("##value", &value, 0.05f, 0.0f, 0.0f, "%.2f");
					}
					break;
				case seri::animation::AnimatorParameterType::boolean:
					{
						bool flag = value != 0.0f;
						if (ImGui::Checkbox("##value", &flag))
						{
							value = flag ? 1.0f : 0.0f;
							changed = true;
						}
					}
					break;
				case seri::animation::AnimatorParameterType::trigger:
					{
						if (animator)
						{
							if (ImGui::RadioButton("##value", value != 0.0f))
							{
								value = 1.0f;
								changed = true;
							}
						}
						else
						{
							ImGui::TextDisabled("trigger");
						}
					}
					break;
			}

			if (changed)
			{
				if (animator)
				{
					animator->parameters[parameter.name] = value;
				}
				else
				{
					parameter.value = value;
					asmAsset.dirty = true;
				}
			}

			ImGui::SameLine(rowStart + rowWidth - buttonWidth);
			if (ImGui::Button("x", ImVec2(buttonWidth, 0.0f)))
			{
				removeIndex = i;
			}

			ImGui::PopID();
		}

		if (removeIndex >= 0)
		{
			DeleteParameter(asmAsset, removeIndex);
		}
	}

	void ASMPanel::DrawGraph(GUIContext& ctx, seri::animation::AnimationStateMachine& asmAsset, int liveState, float progress)
	{
		ImGuiIO& io = ImGui::GetIO();
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		glm::vec2 canvasMin = ToVec2(ImGui::GetCursorScreenPos());
		glm::vec2 canvasSize = glm::max(ToVec2(ImGui::GetContentRegionAvail()), glm::vec2{ 50.0f, 50.0f });
		glm::vec2 canvasMax = canvasMin + canvasSize;
		glm::vec2 origin = glm::floor(canvasMin + canvasSize * 0.5f + _pan);

		ImGui::InvisibleButton("##Canvas", ToImVec2(canvasSize), ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight | ImGuiButtonFlags_MouseButtonMiddle);

		bool hovered = ImGui::IsItemHovered();
		bool active = ImGui::IsItemActive();
		glm::vec2 mouse = ToVec2(io.MousePos) - origin;

		int hoveredNode = hovered ? HitNode(asmAsset, mouse) : kNoNode;
		int hoveredTransition = hovered && hoveredNode == kNoNode ? HitTransition(asmAsset, mouse) : -1;

		if (hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			if (_linkFrom != kNoNode)
			{
				if (hoveredNode >= 0 && hoveredNode != _linkFrom)
				{
					AddTransition(ctx, asmAsset, _linkFrom, hoveredNode);
				}

				_linkFrom = kNoNode;
			}
			else if (hoveredNode != kNoNode)
			{
				Select(ctx, hoveredNode, -1);
				_dragNode = hoveredNode;
			}
			else
			{
				Select(ctx, kNoNode, hoveredTransition);
			}
		}

		if (_dragNode != kNoNode)
		{
			if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
			{
				_dragNode = kNoNode;
			}
			else if (io.MouseDelta.x != 0.0f || io.MouseDelta.y != 0.0f)
			{
				GetNodePosition(asmAsset, _dragNode) += ToVec2(io.MouseDelta);
				asmAsset.dirty = true;
			}
		}

		if (active && (ImGui::IsMouseDragging(ImGuiMouseButton_Right) || ImGui::IsMouseDragging(ImGuiMouseButton_Middle, 0.0f)))
		{
			_pan += ToVec2(io.MouseDelta);
		}

		if (_linkFrom != kNoNode && ImGui::IsKeyPressed(ImGuiKey_Escape, false))
		{
			_linkFrom = kNoNode;
		}

		if (hovered && ImGui::IsMouseReleased(ImGuiMouseButton_Right) && io.MouseDragMaxDistanceSqr[ImGuiMouseButton_Right] < io.MouseDragThreshold * io.MouseDragThreshold)
		{
			if (_linkFrom != kNoNode)
			{
				_linkFrom = kNoNode;
			}
			else
			{
				_contextNode = hoveredNode;
				_contextTransition = hoveredTransition;
				_contextPosition = mouse;

				if (hoveredNode != kNoNode || hoveredTransition >= 0)
				{
					Select(ctx, hoveredNode, hoveredTransition);
				}

				ImGui::OpenPopup("##GraphContext");
			}
		}

		drawList->PushClipRect(ToImVec2(canvasMin), ToImVec2(canvasMax), true);

		drawList->AddRectFilled(ToImVec2(canvasMin), ToImVec2(canvasMax), IM_COL32(28, 28, 32, 255));

		ImU32 gridColor = IM_COL32(255, 255, 255, 12);
		for (float x = std::fmod(origin.x - canvasMin.x, kGridStep); x < canvasSize.x; x += kGridStep)
		{
			drawList->AddLine(ImVec2(canvasMin.x + x, canvasMin.y), ImVec2(canvasMin.x + x, canvasMax.y), gridColor);
		}
		for (float y = std::fmod(origin.y - canvasMin.y, kGridStep); y < canvasSize.y; y += kGridStep)
		{
			drawList->AddLine(ImVec2(canvasMin.x, canvasMin.y + y), ImVec2(canvasMax.x, canvasMin.y + y), gridColor);
		}

		const seri::animation::AnimatorTransition* selectedTransition = nullptr;
		if (ctx.inspectorType == InspectorType::animation_state_machine && ctx.asmTransitionIndex >= 0)
		{
			selectedTransition = &asmAsset.transitions[ctx.asmTransitionIndex];
		}

		for (int i = 0; i < static_cast<int>(asmAsset.transitions.size()); i++)
		{
			const seri::animation::AnimatorTransition& transition = asmAsset.transitions[i];

			glm::vec2 start{};
			glm::vec2 end{};
			if (!GetTransitionSegment(asmAsset, transition, start, end))
			{
				continue;
			}

			bool selected = selectedTransition && selectedTransition->from == transition.from && selectedTransition->to == transition.to;

			ImU32 color = IM_COL32(150, 150, 156, 255);
			if (selected)
			{
				color = IM_COL32(255, 190, 70, 255);
			}
			else if (i == hoveredTransition)
			{
				color = IM_COL32(235, 235, 235, 255);
			}

			DrawArrow(drawList, origin + start, origin + end, color);
		}

		if (_linkFrom != kNoNode)
		{
			DrawArrow(drawList, origin + GetNodePosition(asmAsset, _linkFrom), ToVec2(io.MousePos), IM_COL32(255, 190, 70, 255));
		}

		DrawNode(drawList, ctx, asmAsset, kAnyNode, origin, hoveredNode == kAnyNode, false, 0.0f);

		for (int i = 0; i < static_cast<int>(asmAsset.states.size()); i++)
		{
			DrawNode(drawList, ctx, asmAsset, i, origin, hoveredNode == i, i == liveState, progress);
		}

		drawList->PopClipRect();

		DrawContextMenu(ctx, asmAsset);

		if (ctx.inspectorType == InspectorType::animation_state_machine &&
			ImGui::IsWindowFocused() &&
			!io.WantTextInput &&
			ImGui::IsKeyPressed(ImGuiKey_Delete, false))
		{
			if (ctx.asmNodeIndex >= 0)
			{
				DeleteState(ctx, asmAsset, ctx.asmNodeIndex);
			}
			else if (ctx.asmTransitionIndex >= 0)
			{
				DeleteTransition(ctx, asmAsset, ctx.asmTransitionIndex);
			}
		}
	}

	void ASMPanel::DrawNode(ImDrawList* drawList, const GUIContext& ctx, seri::animation::AnimationStateMachine& asmAsset, int node, const glm::vec2& origin, bool hovered, bool live, float progress)
	{
		std::string name = GetNodeName(asmAsset, node);

		glm::vec2 center = origin + GetNodePosition(asmAsset, node);
		glm::vec2 half = GetNodeSize(asmAsset, node) * 0.5f;
		ImVec2 min = ToImVec2(glm::floor(center - half));
		ImVec2 max = ToImVec2(glm::floor(center + half));

		bool selected = ctx.inspectorType == InspectorType::animation_state_machine && ctx.asmNodeIndex == node;

		ImU32 fill = IM_COL32(62, 62, 70, 255);
		if (node == kAnyNode)
		{
			fill = IM_COL32(36, 110, 100, 255);
		}
		else if (name == asmAsset.entryState)
		{
			fill = IM_COL32(170, 95, 40, 255);
		}

		ImU32 border = IM_COL32(16, 16, 20, 255);
		if (selected)
		{
			border = IM_COL32(255, 190, 70, 255);
		}
		else if (live)
		{
			border = IM_COL32(80, 160, 255, 255);
		}
		else if (hovered)
		{
			border = IM_COL32(200, 200, 206, 255);
		}

		drawList->AddRectFilled(min, max, fill, 6.0f);

		if (live)
		{
			float width = (max.x - min.x - 12.0f) * progress;
			drawList->AddRectFilled(ImVec2(min.x + 6.0f, max.y - 7.0f), ImVec2(min.x + 6.0f + width, max.y - 4.0f), IM_COL32(80, 160, 255, 255), 1.5f);
		}

		drawList->AddRect(min, max, border, 6.0f, 0, selected || live ? 2.0f : 1.0f);

		glm::vec2 textSize = ToVec2(ImGui::CalcTextSize(name.c_str()));
		drawList->AddText(ToImVec2(glm::floor(center - textSize * 0.5f)), IM_COL32(235, 235, 235, 255), name.c_str());
	}

	void ASMPanel::DrawArrow(ImDrawList* drawList, const glm::vec2& start, const glm::vec2& end, ImU32 color)
	{
		glm::vec2 delta = end - start;
		float length = glm::length(delta);
		if (length < 1.0f)
		{
			return;
		}

		glm::vec2 direction = delta / length;
		glm::vec2 normal{ -direction.y, direction.x };
		glm::vec2 middle = (start + end) * 0.5f;

		drawList->AddLine(ToImVec2(start), ToImVec2(end), color, 2.0f);
		drawList->AddTriangleFilled(
			ToImVec2(middle + direction * 7.0f),
			ToImVec2(middle - direction * 5.0f + normal * 6.0f),
			ToImVec2(middle - direction * 5.0f - normal * 6.0f),
			color
		);
	}

	void ASMPanel::DrawContextMenu(GUIContext& ctx, seri::animation::AnimationStateMachine& asmAsset)
	{
		if (!ImGui::BeginPopup("##GraphContext"))
		{
			return;
		}

		if (_contextNode != kNoNode)
		{
			if (ImGui::MenuItem("Make Transition"))
			{
				_linkFrom = _contextNode;
			}

			if (_contextNode >= 0)
			{
				bool isEntry = asmAsset.states[_contextNode].name == asmAsset.entryState;
				if (ImGui::MenuItem("Set as Entry State", nullptr, false, !isEntry))
				{
					asmAsset.entryState = asmAsset.states[_contextNode].name;
					asmAsset.dirty = true;
				}

				if (ImGui::MenuItem("Delete State"))
				{
					DeleteState(ctx, asmAsset, _contextNode);
				}
			}
		}
		else if (_contextTransition >= 0)
		{
			if (ImGui::MenuItem("Delete Transition"))
			{
				DeleteTransition(ctx, asmAsset, _contextTransition);
			}
		}
		else if (ImGui::MenuItem("Create State"))
		{
			CreateState(ctx, asmAsset, _contextPosition);
		}

		ImGui::EndPopup();
	}

	void ASMPanel::DrawStateInspector(GUIContext& ctx, seri::animation::AnimationStateMachine& asmAsset)
	{
		int stateIndex = ctx.asmNodeIndex;
		seri::animation::AnimatorState& state = asmAsset.states[stateIndex];

		ScopedChild scopedChild("##ASMState");

		ImGui::TextUnformatted("State");
		ImGui::Separator();

		bool changed = false;

		ImGui::PushID("Name");
		ImGui::Columns(2, nullptr, false);
		ImGui::SetColumnWidth(0, 90.0f);
		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted("Name");
		ImGui::NextColumn();
		ImGui::SetNextItemWidth(-1);

		std::string name;
		if (DrawNameInput("##value", state.name, name))
		{
			RenameState(asmAsset, stateIndex, name);
		}

		ImGui::Columns(1);
		ImGui::PopID();

		if (state.name == asmAsset.entryState)
		{
			DrawLabel("Entry", "yes", true);
		}
		else if (ImGui::Button("Set as Entry State", ImVec2(-1, 0)))
		{
			asmAsset.entryState = state.name;
			changed = true;
		}

		uint64_t selection = 0;
		if (DrawAssetPicker("Clip Asset", state.clipAssetId, seri::asset::AssetType::mesh, selection))
		{
			state.clipAssetId = selection;
			changed = true;
		}

		std::shared_ptr<seri::Model> model = FindClipModel(ctx, state);
		if (model && !model->animations.empty())
		{
			std::vector<const char*> clipNames;
			int clipIndex = -1;

			for (size_t i = 0; i < model->animations.size(); i++)
			{
				clipNames.push_back(model->animations[i].name.c_str());

				if (model->animations[i].name == state.clip)
				{
					clipIndex = static_cast<int>(i);
				}
			}

			if (DrawCombo("Clip", clipIndex, clipNames.data(), static_cast<int>(clipNames.size())))
			{
				state.clip = model->animations[clipIndex].name;
				changed = true;
			}

			if (clipIndex >= 0)
			{
				DrawLabel("Duration", fmt::format("{:.3f}", model->animations[clipIndex].GetDuration()).c_str(), true);
			}
			else if (!state.clip.empty())
			{
				DrawLabel("Missing", state.clip.c_str(), false);
			}
		}
		else
		{
			changed |= DrawTextInput("Clip", state.clip);
		}

		changed |= DrawBool("Loop", state.loop);
		changed |= DrawFloat("Speed", state.speed, 0.05f, -10.0f, 10.0f);

		if (changed)
		{
			asmAsset.dirty = true;
		}
	}

	void ASMPanel::DrawTransitionInspector(GUIContext& ctx, seri::animation::AnimationStateMachine& asmAsset)
	{
		std::string from = asmAsset.transitions[ctx.asmTransitionIndex].from;
		std::string to = asmAsset.transitions[ctx.asmTransitionIndex].to;

		int count = 0;
		for (const auto& transition : asmAsset.transitions)
		{
			if (transition.from == from && transition.to == to)
			{
				count++;
			}
		}

		int removeIndex = -1;
		int number = 0;

		for (int i = 0; i < static_cast<int>(asmAsset.transitions.size()); i++)
		{
			seri::animation::AnimatorTransition& transition = asmAsset.transitions[i];
			if (transition.from != from || transition.to != to)
			{
				continue;
			}

			number++;

			ImGui::PushID(i);

			{
				ScopedChild scopedChild("##ASMTransition");

				ImGui::AlignTextToFramePadding();
				if (count > 1)
				{
					ImGui::Text("%s -> %s (%d)", from.c_str(), to.c_str(), number);
				}
				else
				{
					ImGui::Text("%s -> %s", from.c_str(), to.c_str());
				}

				float deleteWidth = ImGui::CalcTextSize("Delete").x + ImGui::GetStyle().FramePadding.x * 2.0f;
				ImGui::SameLine();
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - deleteWidth);
				if (ImGui::Button("Delete"))
				{
					removeIndex = i;
				}

				ImGui::Separator();

				bool changed = false;

				changed |= DrawFloat("Fade", transition.fade, 0.01f, 0.0f, 10.0f);
				changed |= DrawBool("Exit Time", transition.exitTime);

				ImGui::SeparatorText("Conditions");

				int removeCondition = -1;

				for (int c = 0; c < static_cast<int>(transition.conditions.size()); c++)
				{
					ImGui::PushID(c);

					bool remove = false;
					changed |= DrawCondition(asmAsset, transition.conditions[c], remove);

					if (remove)
					{
						removeCondition = c;
					}

					ImGui::PopID();
				}

				if (removeCondition >= 0)
				{
					transition.conditions.erase(transition.conditions.begin() + removeCondition);
					changed = true;
				}

				if (transition.conditions.empty() && !transition.exitTime)
				{
					ImGui::TextDisabled("no conditions and no exit time, fires immediately");
				}

				if (ImGui::Button("Add Condition", ImVec2(-1, 0)))
				{
					seri::animation::AnimatorCondition condition;

					if (!asmAsset.parameters.empty())
					{
						const seri::animation::AnimatorParameter& parameter = asmAsset.parameters[0];
						bool isNumber = parameter.type == seri::animation::AnimatorParameterType::number;

						condition.parameter = parameter.name;
						condition.op = isNumber ? seri::animation::AnimatorConditionOp::greater : seri::animation::AnimatorConditionOp::equals;
						condition.value = isNumber ? 0.0f : 1.0f;
					}

					transition.conditions.push_back(condition);
					changed = true;
				}

				if (changed)
				{
					asmAsset.dirty = true;
				}
			}

			ImGui::PopID();
		}

		if (removeIndex < 0)
		{
			return;
		}

		DeleteTransition(ctx, asmAsset, removeIndex);

		for (int i = 0; i < static_cast<int>(asmAsset.transitions.size()); i++)
		{
			if (asmAsset.transitions[i].from == from && asmAsset.transitions[i].to == to)
			{
				Select(ctx, kNoNode, i);
				break;
			}
		}
	}

	void ASMPanel::DrawOverviewInspector(seri::animation::AnimationStateMachine& asmAsset)
	{
		ScopedChild scopedChild("##ASMOverview");

		ImGui::TextUnformatted("Overview");
		ImGui::Separator();

		std::vector<const char*> stateNames;
		int entry = -1;

		for (size_t i = 0; i < asmAsset.states.size(); i++)
		{
			stateNames.push_back(asmAsset.states[i].name.c_str());

			if (asmAsset.states[i].name == asmAsset.entryState)
			{
				entry = static_cast<int>(i);
			}
		}

		if (DrawCombo("Entry", entry, stateNames.data(), static_cast<int>(stateNames.size())))
		{
			asmAsset.entryState = asmAsset.states[entry].name;
			asmAsset.dirty = true;
		}

		DrawLabel("States", std::to_string(asmAsset.states.size()).c_str(), true);
		DrawLabel("Transitions", std::to_string(asmAsset.transitions.size()).c_str(), true);
		DrawLabel("Parameters", std::to_string(asmAsset.parameters.size()).c_str(), true);
	}

	bool ASMPanel::DrawCondition(seri::animation::AnimationStateMachine& asmAsset, seri::animation::AnimatorCondition& condition, bool& remove)
	{
		static const char* opNames[] = { ">", "<", "==", "!=" };
		static const char* boolNames[] = { "false", "true" };

		bool changed = false;

		float buttonWidth = ImGui::GetFrameHeight();
		float spacing = ImGui::GetStyle().ItemSpacing.x;
		float width = ImGui::GetContentRegionAvail().x - buttonWidth - spacing;

		const seri::animation::AnimatorParameter* parameter = asmAsset.FindParameter(condition.parameter);
		bool hasValue = parameter && parameter->type != seri::animation::AnimatorParameterType::trigger;

		ImGui::SetNextItemWidth(hasValue ? width * 0.45f : width);

		const char* preview = condition.parameter.empty() ? "<none>" : condition.parameter.c_str();
		if (ImGui::BeginCombo("##parameter", preview))
		{
			for (const auto& candidate : asmAsset.parameters)
			{
				if (ImGui::Selectable(candidate.name.c_str(), candidate.name == condition.parameter))
				{
					bool isNumber = candidate.type == seri::animation::AnimatorParameterType::number;

					condition.parameter = candidate.name;
					condition.op = isNumber ? seri::animation::AnimatorConditionOp::greater : seri::animation::AnimatorConditionOp::equals;
					condition.value = isNumber ? 0.0f : 1.0f;
					changed = true;
				}
			}

			ImGui::EndCombo();
		}

		if (parameter && parameter->type == seri::animation::AnimatorParameterType::number)
		{
			int op = static_cast<int>(condition.op);

			ImGui::SameLine();
			ImGui::SetNextItemWidth(width * 0.2f);
			if (ImGui::Combo("##op", &op, opNames, IM_ARRAYSIZE(opNames)))
			{
				condition.op = static_cast<seri::animation::AnimatorConditionOp>(op);
				changed = true;
			}

			ImGui::SameLine();
			ImGui::SetNextItemWidth(width * 0.35f - spacing * 2.0f);
			changed |= ImGui::DragFloat("##value", &condition.value, 0.05f, 0.0f, 0.0f, "%.2f");
		}
		else if (parameter && parameter->type == seri::animation::AnimatorParameterType::boolean)
		{
			int value = (condition.op == seri::animation::AnimatorConditionOp::equals) == (condition.value != 0.0f) ? 1 : 0;

			ImGui::SameLine();
			ImGui::SetNextItemWidth(width * 0.55f - spacing);
			if (ImGui::Combo("##value", &value, boolNames, IM_ARRAYSIZE(boolNames)))
			{
				condition.op = seri::animation::AnimatorConditionOp::equals;
				condition.value = static_cast<float>(value);
				changed = true;
			}
		}

		ImGui::SameLine();
		if (ImGui::Button("x", ImVec2(buttonWidth, 0.0f)))
		{
			remove = true;
		}

		return changed;
	}

	bool ASMPanel::DrawNameInput(const char* label, const std::string& name, std::string& result)
	{
		ImGuiID id = ImGui::GetID(label);
		bool editing = _nameEditId == id;

		if (!editing)
		{
			_nameScratch = name;
		}

		ImGui::InputText(label, editing ? &_nameEditBuffer : &_nameScratch);

		if (ImGui::IsItemActivated())
		{
			_nameEditId = id;
			_nameEditBuffer = name;
		}

		if (!ImGui::IsItemDeactivated() || _nameEditId != id)
		{
			return false;
		}

		_nameEditId = 0;

		if (!ImGui::IsItemDeactivatedAfterEdit() || _nameEditBuffer == name)
		{
			return false;
		}

		result = _nameEditBuffer;

		return true;
	}

	void ASMPanel::Select(GUIContext& ctx, int node, int transition)
	{
		ctx.asmNodeIndex = node;
		ctx.asmTransitionIndex = transition;
		ctx.inspectorType = InspectorType::animation_state_machine;
	}

	void ASMPanel::ValidateSelection(GUIContext& ctx, const seri::animation::AnimationStateMachine& asmAsset)
	{
		int stateCount = static_cast<int>(asmAsset.states.size());
		int transitionCount = static_cast<int>(asmAsset.transitions.size());

		if (ctx.asmNodeIndex >= stateCount || ctx.asmNodeIndex < kAnyNode)
		{
			ctx.asmNodeIndex = kNoNode;
		}
		if (ctx.asmTransitionIndex >= transitionCount)
		{
			ctx.asmTransitionIndex = -1;
		}
		if (_dragNode >= stateCount)
		{
			_dragNode = kNoNode;
		}
		if (_linkFrom >= stateCount)
		{
			_linkFrom = kNoNode;
		}
		if (_contextNode >= stateCount)
		{
			_contextNode = kNoNode;
		}
		if (_contextTransition >= transitionCount)
		{
			_contextTransition = -1;
		}
	}

	void ASMPanel::FrameAll(seri::animation::AnimationStateMachine& asmAsset)
	{
		glm::vec2 min = GetNodePosition(asmAsset, kAnyNode);
		glm::vec2 max = min;

		for (int i = 0; i < static_cast<int>(asmAsset.states.size()); i++)
		{
			min = glm::min(min, GetNodePosition(asmAsset, i));
			max = glm::max(max, GetNodePosition(asmAsset, i));
		}

		_pan = -(min + max) * 0.5f;
	}

	void ASMPanel::CreateState(GUIContext& ctx, seri::animation::AnimationStateMachine& asmAsset, const glm::vec2& position)
	{
		std::string name = "New State";
		for (int n = 1; asmAsset.FindState(name) >= 0; n++)
		{
			name = fmt::format("New State {}", n);
		}

		seri::animation::AnimatorState& state = asmAsset.states.emplace_back();
		state.name = name;
		state.position = position;

		if (asmAsset.FindState(asmAsset.entryState) < 0)
		{
			asmAsset.entryState = name;
		}

		asmAsset.dirty = true;

		Select(ctx, static_cast<int>(asmAsset.states.size()) - 1, -1);
	}

	void ASMPanel::DeleteState(GUIContext& ctx, seri::animation::AnimationStateMachine& asmAsset, int stateIndex)
	{
		std::string name = asmAsset.states[stateIndex].name;

		std::erase_if(asmAsset.transitions,
			[&name](const seri::animation::AnimatorTransition& transition)
			{
				return transition.from == name || transition.to == name;
			}
		);

		asmAsset.states.erase(asmAsset.states.begin() + stateIndex);

		if (asmAsset.entryState == name)
		{
			asmAsset.entryState = asmAsset.states.empty() ? "" : asmAsset.states[0].name;
		}

		asmAsset.dirty = true;

		Select(ctx, kNoNode, -1);
	}

	void ASMPanel::RenameState(seri::animation::AnimationStateMachine& asmAsset, int stateIndex, const std::string& name)
	{
		if (name.empty() || name == seri::animation::AnimationStateMachine::kAnyState || asmAsset.FindState(name) >= 0)
		{
			LIB_LOGGER(warning, gui) << "state name '" << name << "' is empty or already used";
			return;
		}

		std::string oldName = asmAsset.states[stateIndex].name;

		for (auto& transition : asmAsset.transitions)
		{
			if (transition.from == oldName)
			{
				transition.from = name;
			}
			if (transition.to == oldName)
			{
				transition.to = name;
			}
		}

		if (asmAsset.entryState == oldName)
		{
			asmAsset.entryState = name;
		}

		asmAsset.states[stateIndex].name = name;
		asmAsset.dirty = true;
	}

	void ASMPanel::AddTransition(GUIContext& ctx, seri::animation::AnimationStateMachine& asmAsset, int from, int to)
	{
		seri::animation::AnimatorTransition& transition = asmAsset.transitions.emplace_back();
		transition.from = GetNodeName(asmAsset, from);
		transition.to = GetNodeName(asmAsset, to);
		transition.exitTime = true;

		asmAsset.dirty = true;

		Select(ctx, kNoNode, static_cast<int>(asmAsset.transitions.size()) - 1);
	}

	void ASMPanel::DeleteTransition(GUIContext& ctx, seri::animation::AnimationStateMachine& asmAsset, int transitionIndex)
	{
		asmAsset.transitions.erase(asmAsset.transitions.begin() + transitionIndex);
		asmAsset.dirty = true;

		Select(ctx, kNoNode, -1);
	}

	void ASMPanel::AddParameter(seri::animation::AnimationStateMachine& asmAsset, seri::animation::AnimatorParameterType type)
	{
		std::string name = "New Parameter";
		for (int n = 1; asmAsset.FindParameter(name); n++)
		{
			name = fmt::format("New Parameter {}", n);
		}

		seri::animation::AnimatorParameter& parameter = asmAsset.parameters.emplace_back();
		parameter.name = name;
		parameter.type = type;

		asmAsset.dirty = true;
	}

	void ASMPanel::RenameParameter(seri::animation::AnimationStateMachine& asmAsset, int parameterIndex, const std::string& name)
	{
		if (name.empty() || asmAsset.FindParameter(name))
		{
			LIB_LOGGER(warning, gui) << "parameter name '" << name << "' is empty or already used";
			return;
		}

		std::string oldName = asmAsset.parameters[parameterIndex].name;

		for (auto& transition : asmAsset.transitions)
		{
			for (auto& condition : transition.conditions)
			{
				if (condition.parameter == oldName)
				{
					condition.parameter = name;
				}
			}
		}

		asmAsset.parameters[parameterIndex].name = name;
		asmAsset.dirty = true;
	}

	void ASMPanel::DeleteParameter(seri::animation::AnimationStateMachine& asmAsset, int parameterIndex)
	{
		std::string name = asmAsset.parameters[parameterIndex].name;

		for (auto& transition : asmAsset.transitions)
		{
			std::erase_if(transition.conditions,
				[&name](const seri::animation::AnimatorCondition& condition)
				{
					return condition.parameter == name;
				}
			);
		}

		asmAsset.parameters.erase(asmAsset.parameters.begin() + parameterIndex);
		asmAsset.dirty = true;
	}

	entt::entity ASMPanel::FindPreviewEntity(const GUIContext& ctx)
	{
		auto scene = seri::scene::SceneManager::GetActiveScene();
		auto& registry = seri::scene::SceneManager::GetRegistry();

		if (scene && ctx.selectedEntityId != 0 && scene->HasEntity(ctx.selectedEntityId))
		{
			entt::entity entity = scene->GetEntityByID(ctx.selectedEntityId);

			auto* animator = registry.try_get<seri::component::AnimatorComponent>(entity);
			if (animator && animator->asmAssetId == ctx.asmAssetId)
			{
				return entity;
			}
		}

		for (entt::entity entity : registry.view<seri::component::AnimatorComponent>())
		{
			if (registry.get<seri::component::AnimatorComponent>(entity).asmAssetId == ctx.asmAssetId)
			{
				return entity;
			}
		}

		return entt::null;
	}

	std::shared_ptr<seri::Model> ASMPanel::FindClipModel(const GUIContext& ctx, const seri::animation::AnimatorState& state)
	{
		uint64_t modelId = state.clipAssetId;

		if (modelId == 0)
		{
			entt::entity entity = FindPreviewEntity(ctx);
			if (entity != entt::null)
			{
				auto& registry = seri::scene::SceneManager::GetRegistry();

				modelId = registry.get<seri::component::AnimatorComponent>(entity).clipAssetId;

				auto* renderer = registry.try_get<seri::component::SkinnedMeshRendererComponent>(entity);
				if (modelId == 0 && renderer)
				{
					modelId = renderer->meshAssetId;
				}
			}
		}

		return modelId != 0 ? seri::asset::AssetManager::GetAssetByID<seri::Model>(modelId) : nullptr;
	}

	int ASMPanel::HitNode(seri::animation::AnimationStateMachine& asmAsset, const glm::vec2& point)
	{
		auto contains = [&](int node)
			{
				glm::vec2 delta = glm::abs(point - GetNodePosition(asmAsset, node));
				glm::vec2 half = GetNodeSize(asmAsset, node) * 0.5f;
				return delta.x <= half.x && delta.y <= half.y;
			};

		for (int i = static_cast<int>(asmAsset.states.size()) - 1; i >= 0; i--)
		{
			if (contains(i))
			{
				return i;
			}
		}

		return contains(kAnyNode) ? kAnyNode : kNoNode;
	}

	int ASMPanel::HitTransition(seri::animation::AnimationStateMachine& asmAsset, const glm::vec2& point)
	{
		for (int i = 0; i < static_cast<int>(asmAsset.transitions.size()); i++)
		{
			glm::vec2 start{};
			glm::vec2 end{};
			if (!GetTransitionSegment(asmAsset, asmAsset.transitions[i], start, end))
			{
				continue;
			}

			glm::vec2 segment = end - start;
			float t = glm::clamp(glm::dot(point - start, segment) / glm::dot(segment, segment), 0.0f, 1.0f);

			if (glm::length(point - (start + segment * t)) <= kTransitionHitDistance)
			{
				return i;
			}
		}

		return -1;
	}

	bool ASMPanel::GetTransitionSegment(seri::animation::AnimationStateMachine& asmAsset, const seri::animation::AnimatorTransition& transition, glm::vec2& start, glm::vec2& end)
	{
		int from = transition.from == seri::animation::AnimationStateMachine::kAnyState ? kAnyNode : asmAsset.FindState(transition.from);
		int to = asmAsset.FindState(transition.to);

		if (from == kNoNode || to < 0 || from == to)
		{
			return false;
		}

		start = GetNodePosition(asmAsset, from);
		end = GetNodePosition(asmAsset, to);

		glm::vec2 delta = end - start;
		float length = glm::length(delta);
		if (length < 1.0f)
		{
			return false;
		}

		glm::vec2 offset = glm::vec2{ -delta.y, delta.x } / length * kTransitionOffset;
		start += offset;
		end += offset;

		return true;
	}

	glm::vec2& ASMPanel::GetNodePosition(seri::animation::AnimationStateMachine& asmAsset, int node)
	{
		return node == kAnyNode ? asmAsset.anyStatePosition : asmAsset.states[node].position;
	}

	glm::vec2 ASMPanel::GetNodeSize(seri::animation::AnimationStateMachine& asmAsset, int node)
	{
		float textWidth = ImGui::CalcTextSize(GetNodeName(asmAsset, node).c_str()).x;

		return { std::max(kNodeMinWidth, textWidth + 32.0f), kNodeHeight };
	}

	std::string ASMPanel::GetNodeName(seri::animation::AnimationStateMachine& asmAsset, int node)
	{
		return node == kAnyNode ? std::string{ seri::animation::AnimationStateMachine::kAnyState } : asmAsset.states[node].name;
	}

	ImVec2 ASMPanel::ToImVec2(const glm::vec2& v)
	{
		return ImVec2(v.x, v.y);
	}

	glm::vec2 ASMPanel::ToVec2(const ImVec2& v)
	{
		return { v.x, v.y };
	}
}
