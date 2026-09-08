#include "gui/panels/ScenePanel.h"

namespace seri::editor
{
	const char* ScenePanel::kShadingModeNames[ScenePanel::kShadingModeCount] = {
		"Shaded",
		"Wireframe",
	};

	void ScenePanel::Draw(GUIContext& ctx)
	{
		ShowOptions();

		ImGui::Separator();

		ImVec2 panelSize = ImGui::GetContentRegionAvail();
		if (panelSize.x <= 0.0f || panelSize.y <= 0.0f)
		{
			return;
		}

		float fbAspect = seri::RenderingManager::GetEditorRT()->GetAspectRatio();
		if (fbAspect <= 0.0f)
		{
			return;
		}

		float panelAspect = panelSize.x / panelSize.y;

		ImVec2 imageSize;
		if (panelAspect > fbAspect)
		{
			imageSize.y = panelSize.y;
			imageSize.x = panelSize.y * fbAspect;
		}
		else
		{
			imageSize.x = panelSize.x;
			imageSize.y = panelSize.x / fbAspect;
		}

		ImVec2 cursorPos = ImGui::GetCursorScreenPos();

		float offsetX = (panelSize.x - imageSize.x) * 0.5f;
		float offsetY = (panelSize.y - imageSize.y) * 0.5f;

		ImVec2 drawPos = ImVec2(cursorPos.x + offsetX, cursorPos.y + offsetY);
		ImVec2 imageMin = drawPos;
		ImVec2 imageMax = ImVec2(drawPos.x + imageSize.x, drawPos.y + imageSize.y);

		auto tex = (ImTextureID)(intptr_t)seri::RenderingManager::GetEditorRT()->GetFirstColorTextureHandle();

		ImGui::GetWindowDrawList()->AddImage(
			tex,
			imageMin,
			imageMax,
			ImVec2(0, 1),
			ImVec2(1, 0)
		);

		if (seri::scene::SceneManager::GetState() != seri::scene::SceneState::edit)
		{
			return;
		}

		ShowGizmo(imageMin, imageSize);
		ShowEntityGizmo(ctx, imageMin, imageSize);
		ShowGizmoToolbar(imageMin);
		ControlMove(imageMin, imageMax);
	}

	void ScenePanel::ShowOptions()
	{
		ImGui::SetNextItemWidth(140.0f);

		if (ImGui::BeginCombo("##SceneShading", kShadingModeNames[static_cast<int>(_shadingMode)]))
		{
			for (int i = 0; i < kShadingModeCount; i++)
			{
				if (ImGui::Selectable(kShadingModeNames[i], static_cast<int>(_shadingMode) == i))
				{
					_shadingMode = static_cast<ShadingMode>(i);
					seri::RenderingManager::SetEditorWireframe(_shadingMode == ShadingMode::wireframe);
				}
			}

			ImGui::EndCombo();
		}
	}

	void ScenePanel::ControlMove(const ImVec2& imageMin, const ImVec2& imageMax)
	{
		static bool lookActive = false;
		static bool panActive = false;
		static ImVec2 lastMouse = ImVec2(0, 0);
		static double anchorX = 0.0;
		static double anchorY = 0.0;

		ImVec2 mouse = ImGui::GetMousePos();

		bool panelActive =
			ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) ||
			ImGui::IsWindowFocused();
		bool mouseInside =
			mouse.x >= imageMin.x && mouse.x <= imageMax.x &&
			mouse.y >= imageMin.y && mouse.y <= imageMax.y;

		if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && panelActive && mouseInside)
		{
			lookActive = true;

			auto cursor = seri::WindowManager::GetCursorPosition();
			anchorX = cursor.first;
			anchorY = cursor.second;
		}
		if (ImGui::IsMouseReleased(ImGuiMouseButton_Right))
		{
			lookActive = false;
		}

		if (ImGui::IsMouseClicked(ImGuiMouseButton_Middle) && panelActive && mouseInside)
		{
			panActive = true;
			lastMouse = mouse;
		}
		if (ImGui::IsMouseReleased(ImGuiMouseButton_Middle))
		{
			panActive = false;
		}

		auto camera = seri::Graphic::GetEditorCamera();

		if (lookActive)
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_None);

			auto cursor = seri::WindowManager::GetCursorPosition();

			camera->Look(
				static_cast<float>(cursor.first - anchorX),
				static_cast<float>(cursor.second - anchorY)
			);

			seri::WindowManager::SetCursorPosition(anchorX, anchorY);

			glm::vec3 direction{ 0.0f, 0.0f, 0.0f };
			if (ImGui::IsKeyDown(ImGuiKey_W))
			{
				direction.z += 1.0f;
			}
			if (ImGui::IsKeyDown(ImGuiKey_S))
			{
				direction.z -= 1.0f;
			}
			if (ImGui::IsKeyDown(ImGuiKey_A))
			{
				direction.x -= 1.0f;
			}
			if (ImGui::IsKeyDown(ImGuiKey_D))
			{
				direction.x += 1.0f;
			}
			if (ImGui::IsKeyDown(ImGuiKey_E))
			{
				direction.y += 1.0f;
			}
			if (ImGui::IsKeyDown(ImGuiKey_Q))
			{
				direction.y -= 1.0f;
			}

			camera->Move(direction, seri::TimeWrapper::GetDeltaTime(), ImGui::GetIO().KeyShift);
		}
		else if (panActive)
		{
			ImVec2 delta = { mouse.x - lastMouse.x, mouse.y - lastMouse.y };
			lastMouse = mouse;

			camera->Pan(delta.x, delta.y, imageMax.y - imageMin.y);
		}

		if (!lookActive && panelActive && mouseInside && ImGui::GetIO().MouseWheel != 0.0f)
		{
			camera->Zoom(ImGui::GetIO().MouseWheel);
		}
	}

	void ScenePanel::ShowGizmoToolbar(const ImVec2& imageMin)
	{
		ImGui::SetNextWindowPos(ImVec2(imageMin.x + 5.0f, imageMin.y + 5.0f));
		ImGui::SetNextWindowBgAlpha(0.85f);

		ImGuiWindowFlags flags =
			ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_NoDocking |
			ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoFocusOnAppearing |
			ImGuiWindowFlags_NoNav;

		ImGui::Begin("##SceneGizmoToolbar", nullptr, flags);

		ImGui::TextUnformatted("Gizmo");
		ImGui::Separator();

		int space = static_cast<int>(_gizmoSpace);
		ImGui::RadioButton("Local", &space, static_cast<int>(GizmoSpace::local));
		ImGui::RadioButton("World", &space, static_cast<int>(GizmoSpace::world));
		_gizmoSpace = static_cast<GizmoSpace>(space);

		ImGui::Separator();

		int op = static_cast<int>(_gizmoOperation);
		ImGui::RadioButton("Translate", &op, static_cast<int>(GizmoOperation::translate));
		ImGui::RadioButton("Rotate", &op, static_cast<int>(GizmoOperation::rotate));
		ImGui::RadioButton("Scale", &op, static_cast<int>(GizmoOperation::scale));
		_gizmoOperation = static_cast<GizmoOperation>(op);

		ImGui::End();
	}

	void ScenePanel::ShowGizmo(const ImVec2& imageMin, const ImVec2& imageSize)
	{
		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(
			imageMin.x,
			imageMin.y,
			imageSize.x,
			imageSize.y
		);

		float gizmoSize = 64.0f;
		float padding = 5.0f;

		float x = imageMin.x + imageSize.x - gizmoSize - padding;
		float y = imageMin.y + padding;

		auto camera = seri::Graphic::GetEditorCamera();

		glm::mat4 view = camera->GetView();

		ImGuizmo::PushID("scene_gizmo");

		ImGuizmo::ViewManipulate(
			glm::value_ptr(view),
			8.0f,
			ImVec2(x, y),
			ImVec2(gizmoSize, gizmoSize),
			0x10101010
		);

		if (ImGuizmo::IsUsingViewManipulate())
		{
			camera->SetFromViewMatrix(view);
		}

		ImGuizmo::PopID();
	}

	void ScenePanel::ShowEntityGizmo(GUIContext& ctx, const ImVec2& imageMin, const ImVec2& imageSize)
	{
		if (ctx.inspectorType != InspectorType::entity || ctx.selectedEntityId == 0)
		{
			return;
		}

		ImGuizmo::MODE mode = ImGuizmo::WORLD;
		switch (_gizmoSpace)
		{
			case GizmoSpace::local:
				mode = ImGuizmo::LOCAL;
				break;
			case GizmoSpace::world:
				mode = ImGuizmo::WORLD;
				break;
		}

		ImGuizmo::OPERATION operation = ImGuizmo::TRANSLATE;
		switch (_gizmoOperation)
		{
			case GizmoOperation::translate:
				operation = ImGuizmo::TRANSLATE;
				break;
			case GizmoOperation::rotate:
				operation = ImGuizmo::ROTATE;
				break;
			case GizmoOperation::scale:
				operation = ImGuizmo::SCALE;
				break;
		}

		if (_gizmoOperation == GizmoOperation::rotate)
		{
			mode = ImGuizmo::LOCAL;
			_gizmoSpace = GizmoSpace::local;
		}

		auto scene = seri::scene::SceneManager::GetActiveScene();
		auto& registry = seri::scene::SceneManager::GetRegistry();
		auto entity = scene->GetEntityByID(ctx.selectedEntityId);

		auto* idComp = registry.try_get<seri::component::IDComponent>(entity);
		auto* transformComp = registry.try_get<seri::component::TransformComponent>(entity);
		if (idComp == nullptr || transformComp == nullptr)
		{
			return;
		}

		glm::mat4 parentWorld{ 1.0f };
		if (idComp->parentId != 0)
		{
			auto parentEntity = scene->GetEntityByID(idComp->parentId);
			auto* parentTransformComp = registry.try_get<seri::component::TransformComponent>(parentEntity);
			parentWorld = parentTransformComp->worldMatrix;
		}

		glm::mat4 worldMatrix = parentWorld * transformComp->localMatrix;
		glm::vec3 lockedWorldPos = glm::vec3(worldMatrix[3]);

		ImGuizmo::PushID("scene_entity_gizmo");

		ImGuizmo::Manipulate(
			glm::value_ptr(Graphic::GetEditorCamera()->GetView()),
			glm::value_ptr(Graphic::GetEditorCamera()->GetProjection()),
			operation,
			mode,
			glm::value_ptr(worldMatrix),
			nullptr
		);

		if (ImGuizmo::IsUsing())
		{
			if (_gizmoOperation == GizmoOperation::rotate)
			{
				worldMatrix[3] = glm::vec4(lockedWorldPos, 1.0f);
			}

			Util::Decompose(
				glm::inverse(parentWorld) * worldMatrix,
				transformComp->position,
				transformComp->rotation,
				transformComp->scale
			);

			scene->SetAsDirty();
		}

		ImGuizmo::PopID();
	}
}
