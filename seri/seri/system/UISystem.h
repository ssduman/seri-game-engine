#pragma once

#include "seri/scene/SceneManager.h"

#include <entt/entt.hpp>

#include <unordered_map>

namespace seri::system
{
	class UISystem
	{
	public:
		static void Update();

		static void DrawRectGizmo(entt::entity entity);

		static bool IsScreenSpace(entt::entity entity);

		static int GetDrawOrder(entt::entity entity);

		static glm::vec2 GetScreenSize();

		static void SetPointerViewport(const glm::vec2& min, const glm::vec2& size, bool enabled);

		static bool GetPointerPosition(glm::vec2& position);

	private:
		static void Collect(const seri::scene::SceneTreeNode& node, bool inScreenCanvas, const glm::mat4& parentFrame, const glm::vec2& parentSize, int& order);

		static void UpdateButtons();

		static void SyncCamera();

		static glm::vec2 ResolveScreenSize();

		static inline glm::vec2 _screenSize{ 1920.0f, 1080.0f };

		static inline glm::vec2 _pointerViewportMin{ 0.0f, 0.0f };
		static inline glm::vec2 _pointerViewportSize{ 0.0f, 0.0f };
		static inline bool _pointerViewportSet{ false };
		static inline bool _pointerEnabled{ true };

		static inline const int kCanvasOrderStride = 1 << 16;

		static inline std::unordered_map<entt::entity, int> _uiEntityDrawOrders{};

	};
}
