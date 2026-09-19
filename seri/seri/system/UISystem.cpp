#include "Seripch.h"
#include "seri/system/UISystem.h"

#include "seri/component/Components.h"
#include "seri/graphic/Graphic.h"
#include "seri/rendering/render/RenderingManager.h"
#include "seri/draw/DebugDraw.h"
#include "seri/input/InputManager.h"
#include "seri/window/WindowManager.h"

#include <limits>

namespace seri::system
{
	void UISystem::Update()
	{
		_uiEntityDrawOrders.clear();
		_screenSize = ResolveScreenSize();

		SyncCamera();

		const auto& tree = seri::scene::SceneManager::GetActiveScene()->GetSceneTreeRoot();

		int order = 0;
		bool inScreenCanvas = false;
		glm::mat4 frame = glm::mat4{ 1.0f };
		glm::vec2 size = _screenSize;

		for (const auto& child : tree.children)
		{
			Collect(child, inScreenCanvas, frame, size, order);
		}

		UpdateButtons();
	}

	void UISystem::SetPointerViewport(const glm::vec2& min, const glm::vec2& size, bool enabled)
	{
		_pointerViewportMin = min;
		_pointerViewportSize = size;
		_pointerViewportSet = true;
		_pointerEnabled = enabled;
	}

	bool UISystem::GetPointerPosition(glm::vec2& position)
	{
		glm::vec2 min = _pointerViewportMin;
		glm::vec2 size = _pointerViewportSize;

		if (!_pointerViewportSet)
		{
			min = glm::vec2{ 0.0f, 0.0f };
			size = glm::vec2{ static_cast<float>(WindowManager::GetWidth()), static_cast<float>(WindowManager::GetHeight()) };
		}

		if (!_pointerEnabled || size.x <= 0.0f || size.y <= 0.0f)
		{
			return false;
		}

		glm::vec2 local = (InputManager::GetCursorPosition() - min) / size;
		if (local.x < 0.0f || local.x > 1.0f || local.y < 0.0f || local.y > 1.0f)
		{
			return false;
		}

		position = glm::vec2{ (local.x - 0.5f) * _screenSize.x, (0.5f - local.y) * _screenSize.y };

		return true;
	}

	void UISystem::UpdateButtons()
	{
		auto& registry = seri::scene::SceneManager::GetRegistry();
		auto view = registry.view<seri::component::TransformComponent, seri::component::RectComponent, seri::component::ButtonComponent>();

		bool playing = seri::scene::SceneManager::GetState() == seri::scene::SceneState::play;

		glm::vec2 pointer{ 0.0f, 0.0f };
		bool hasPointer = playing && GetPointerPosition(pointer);

		entt::entity target = entt::null;
		int targetOrder = std::numeric_limits<int>::min();

		for (entt::entity entity : view)
		{
			auto& transform = view.get<seri::component::TransformComponent>(entity);
			auto& rect = view.get<seri::component::RectComponent>(entity);
			auto& button = view.get<seri::component::ButtonComponent>(entity);

			if (!hasPointer || !button.interactable || !transform.isActiveInHierarchy || !IsScreenSpace(entity))
			{
				continue;
			}

			glm::vec4 local = glm::inverse(transform.worldMatrix) * glm::vec4{ pointer, 0.0f, 1.0f };
			glm::vec2 min = -rect.pivot * rect.resolvedSize;
			glm::vec2 max = (glm::vec2{ 1.0f, 1.0f } - rect.pivot) * rect.resolvedSize;

			if (local.x < min.x || local.x > max.x || local.y < min.y || local.y > max.y)
			{
				continue;
			}

			int order = GetDrawOrder(entity);
			if (order >= targetOrder)
			{
				target = entity;
				targetOrder = order;
			}
		}

		bool down = playing && InputManager::IsMouseButtonDown(MouseButtonCode::button_left);
		bool up = playing && InputManager::IsMouseButtonUp(MouseButtonCode::button_left);

		for (entt::entity entity : view)
		{
			auto& button = view.get<seri::component::ButtonComponent>(entity);

			button.hovered = entity == target;
			button.clicked = false;

			if (!playing || !button.interactable)
			{
				button.pressed = false;
				continue;
			}

			if (down && button.hovered)
			{
				button.pressed = true;
			}

			if (up)
			{
				button.clicked = button.pressed && button.hovered;
				button.pressed = false;
			}
		}
	}

	bool UISystem::IsScreenSpace(entt::entity entity)
	{
		return _uiEntityDrawOrders.find(entity) != _uiEntityDrawOrders.end();
	}

	int UISystem::GetDrawOrder(entt::entity entity)
	{
		auto it = _uiEntityDrawOrders.find(entity);
		if (it == _uiEntityDrawOrders.end())
		{
			return 0;
		}

		return it->second;
	}

	glm::vec2 UISystem::GetScreenSize()
	{
		return _screenSize;
	}

	void UISystem::Collect(const seri::scene::SceneTreeNode& node, bool inScreenCanvas, const glm::mat4& parentFrame, const glm::vec2& parentSize, int& order)
	{
		auto& registry = seri::scene::SceneManager::GetRegistry();

		entt::entity entity = seri::scene::SceneManager::GetActiveScene()->GetEntityByID(node.id);

		glm::mat4 frame = parentFrame;
		glm::vec2 size = parentSize;

		if (auto* canvas = registry.try_get<seri::component::CanvasComponent>(entity))
		{
			inScreenCanvas = canvas->mode == seri::ui::CanvasRenderMode::screen_space;

			if (inScreenCanvas)
			{
				frame = glm::mat4{ 1.0f };
				size = _screenSize;
				order = canvas->sortOrder * kCanvasOrderStride;
			}
		}

		if (inScreenCanvas)
		{
			_uiEntityDrawOrders[entity] = order++;

			auto* transform = registry.try_get<seri::component::TransformComponent>(entity);
			auto* rect = registry.try_get<seri::component::RectComponent>(entity);

			if (transform != nullptr && rect != nullptr)
			{
				glm::vec2 parentMin = parentSize * -0.5f;
				glm::vec2 anchorMinPos = parentMin + rect->anchorMin * parentSize;
				glm::vec2 anchorMaxPos = parentMin + rect->anchorMax * parentSize;
				glm::vec2 anchorSpan = anchorMaxPos - anchorMinPos;

				size = anchorSpan + rect->sizeDelta;

				glm::vec2 pivotPos = anchorMinPos + anchorSpan * rect->pivot + rect->anchoredPosition;

				glm::mat4 local = glm::translate(glm::mat4{ 1.0f }, glm::vec3{ pivotPos, transform->position.z });
				local = local * glm::mat4_cast(Util::ToQuaternion(transform->rotation));
				local = glm::scale(local, glm::vec3{ transform->scale.x, transform->scale.y, 1.0f });

				rect->resolvedSize = size;
				rect->resolvedAnchorMinPos = anchorMinPos;
				rect->resolvedAnchorMaxPos = anchorMaxPos;
				rect->resolvedParentFrame = parentFrame;

				frame = parentFrame * local;
				transform->worldMatrix = frame;
			}
			else if (transform != nullptr)
			{
				frame = transform->worldMatrix;
			}
		}

		for (const auto& child : node.children)
		{
			Collect(child, inScreenCanvas, frame, size, order);
		}
	}

	void UISystem::DrawRectGizmo(entt::entity entity)
	{
		auto& registry = seri::scene::SceneManager::GetRegistry();

		auto* transform = registry.try_get<seri::component::TransformComponent>(entity);
		auto* rect = registry.try_get<seri::component::RectComponent>(entity);
		if (transform == nullptr || rect == nullptr)
		{
			return;
		}

		glm::vec4 outlineColor{ 0.2f, 0.8f, 1.0f, 1.0f };
		glm::vec4 anchorColor{ 1.0f, 0.8f, 0.2f, 1.0f };

		glm::vec2 size = rect->resolvedSize;
		glm::vec2 pivot = rect->pivot;

		float x0 = -pivot.x * size.x;
		float x1 = (1.0f - pivot.x) * size.x;
		float y0 = -pivot.y * size.y;
		float y1 = (1.0f - pivot.y) * size.y;

		auto toWorld = [&transform](float x, float y)
			{
				return glm::vec3{ transform->worldMatrix * glm::vec4{ x, y, 0.0f, 1.0f } };
			};

		glm::vec3 bl = toWorld(x0, y0);
		glm::vec3 br = toWorld(x1, y0);
		glm::vec3 tr = toWorld(x1, y1);
		glm::vec3 tl = toWorld(x0, y1);

		debug::DebugDraw::LineUI(bl, br, outlineColor);
		debug::DebugDraw::LineUI(br, tr, outlineColor);
		debug::DebugDraw::LineUI(tr, tl, outlineColor);
		debug::DebugDraw::LineUI(tl, bl, outlineColor);

		const float pivotSize = 6.0f;
		glm::vec3 pivotPos = toWorld(0.0f, 0.0f);
		debug::DebugDraw::LineUI(pivotPos - glm::vec3{ pivotSize, 0.0f, 0.0f }, pivotPos + glm::vec3{ pivotSize, 0.0f, 0.0f }, outlineColor);
		debug::DebugDraw::LineUI(pivotPos - glm::vec3{ 0.0f, pivotSize, 0.0f }, pivotPos + glm::vec3{ 0.0f, pivotSize, 0.0f }, outlineColor);

		const float anchorSize = 10.0f;
		glm::vec2 anchors[4] = {
			{ rect->resolvedAnchorMinPos.x, rect->resolvedAnchorMinPos.y },
			{ rect->resolvedAnchorMaxPos.x, rect->resolvedAnchorMinPos.y },
			{ rect->resolvedAnchorMaxPos.x, rect->resolvedAnchorMaxPos.y },
			{ rect->resolvedAnchorMinPos.x, rect->resolvedAnchorMaxPos.y },
		};

		for (const glm::vec2& anchor : anchors)
		{
			glm::vec3 center = glm::vec3{ rect->resolvedParentFrame * glm::vec4{ anchor, 0.0f, 1.0f } };

			debug::DebugDraw::LineUI(
				center + glm::vec3{ -anchorSize, -anchorSize, 0.0f },
				center + glm::vec3{ anchorSize, anchorSize, 0.0f },
				anchorColor
			);
			debug::DebugDraw::LineUI(
				center + glm::vec3{ -anchorSize, anchorSize, 0.0f },
				center + glm::vec3{ anchorSize, -anchorSize, 0.0f },
				anchorColor
			);
		}
	}

	glm::vec2 UISystem::ResolveScreenSize()
	{
		for (const auto& rt : { seri::RenderingManager::GetGameRT(), seri::RenderingManager::GetEditorRT() })
		{
			if (rt && rt->GetWidth() > 0 && rt->GetHeight() > 0)
			{
				return glm::vec2{ static_cast<float>(rt->GetWidth()), static_cast<float>(rt->GetHeight()) };
			}
		}

		return glm::vec2{ 1920.0f, 1080.0f };
	}

	void UISystem::SyncCamera()
	{
		auto camera = seri::Graphic::GetCameraUI();
		if (!camera)
		{
			return;
		}

		CameraProperties& properties = camera->GetCameraProperties();

		properties.width = _screenSize.x;
		properties.height = _screenSize.y;
		properties.aspect = _screenSize.y > 0.0f ? _screenSize.x / _screenSize.y : 1.0f;

		camera->Update();
	}
}
