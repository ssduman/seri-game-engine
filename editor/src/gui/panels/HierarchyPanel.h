#pragma once

#include "gui/GUIContext.h"

namespace seri::editor
{
	class HierarchyPanel
	{
	public:
		void Draw(GUIContext& ctx);

	private:
		void DrawNode(GUIContext& ctx, const std::shared_ptr<seri::scene::Scene>& activeScene, seri::scene::SceneTreeNode& node, uint64_t& selectedId);

		void ShowAddMenu(const std::shared_ptr<seri::scene::Scene>& activeScene, uint64_t parentId);

		uint64_t _pendingDeleteEntityId{ 0 };
		uint64_t _pendingExpandEntityId{ 0 };

	};
}
