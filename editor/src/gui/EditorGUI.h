#pragma once

#include "gui/GUIContext.h"
#include "gui/panels/ConsolePanel.h"
#include "gui/panels/HierarchyPanel.h"
#include "gui/panels/InspectorPanel.h"
#include "gui/panels/ProjectPanel.h"
#include "gui/panels/ScenePanel.h"
#include "gui/panels/TitleBar.h"

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

		void ProcessEvent(const void* event);

	private:
		void CheckShortcuts();

		void DrawLayout();

		void BuildDefaultDockLayout(ImGuiID dockspaceId);

		GUIContext _context;

		TitleBar _titleBar;
		HierarchyPanel _hierarchyPanel;
		ScenePanel _scenePanel;
		InspectorPanel _inspectorPanel;
		ProjectPanel _projectPanel;
		ConsolePanel _consolePanel;

	};
}
