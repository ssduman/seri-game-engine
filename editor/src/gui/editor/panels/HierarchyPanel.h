#pragma once

#include "gui/editor/GUIContext.h"

namespace seri::editor
{
	class HierarchyPanel
	{
	public:
		void Draw(GUIContext& ctx);

	private:
		void DrawNode(GUIContext& ctx, const std::shared_ptr<seri::scene::Scene>& activeScene, seri::scene::SceneTreeNode& node, uint64_t& selectedId);

		void ShowAddMenu(const std::shared_ptr<seri::scene::Scene>& activeScene, uint64_t parentId);

		entt::entity AddEntity(const std::shared_ptr<seri::scene::Scene>& activeScene, uint64_t parentId, const char* name, std::initializer_list<std::string_view> compNames);

		void AddButton(const std::shared_ptr<seri::scene::Scene>& activeScene, uint64_t parentId);

		uint64_t _pendingDeleteEntityId{ 0 };
		uint64_t _pendingExpandEntityId{ 0 };

	};
}
