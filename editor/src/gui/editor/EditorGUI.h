#pragma once

#include "gui/editor/GUIContext.h"
#include "gui/editor/panels/ConsolePanel.h"
#include "gui/editor/panels/GamePanel.h"
#include "gui/editor/panels/HierarchyPanel.h"
#include "gui/editor/panels/InspectorPanel.h"
#include "gui/editor/panels/ProjectPanel.h"
#include "gui/editor/panels/ScenePanel.h"
#include "gui/editor/panels/TitleBar.h"

namespace seri::editor
{
	class EditorGUI : public seri::Object
	{
	public:
		EditorGUI();

		~EditorGUI() override;

		void Init() override;

		void Update() override;

		void Render() override;

	private:
		void CheckShortcuts();

		void DrawLayout();

		void BuildDefaultDockLayout(ImGuiID dockspaceId);

		GUIContext _context;

		TitleBar _titleBar;
		HierarchyPanel _hierarchyPanel;
		ScenePanel _scenePanel;
		GamePanel _gamePanel;
		InspectorPanel _inspectorPanel;
		ProjectPanel _projectPanel;
		ConsolePanel _consolePanel;

	};
}
