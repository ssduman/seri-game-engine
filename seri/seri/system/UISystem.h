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

	private:
		static void Collect(const seri::scene::SceneTreeNode& node, bool inScreenCanvas, const glm::mat4& parentFrame, const glm::vec2& parentSize, int& order);

		static void SyncCamera();

		static glm::vec2 ResolveScreenSize();

		static inline glm::vec2 _screenSize{ 1920.0f, 1080.0f };

		static inline const int kCanvasOrderStride = 1 << 16;

		static inline std::unordered_map<entt::entity, int> _uiEntityDrawOrders{};

	};
}
