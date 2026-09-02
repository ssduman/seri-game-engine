#pragma once

#include <seri/core/Seri.h>

#include "gui/EditorGUI.h"

namespace seri::editor
{
	class EditorLayer : public seri::LayerBase
	{
	public:
		EditorLayer() : LayerBase("GUILayer")
		{
			editorGUI = std::make_shared<EditorGUI>();
			editorGUI->Init();

			seri::WindowManager::Instance()->AddProcessEventDelegate(
				[&](const void* event)
				{
					editorGUI->ProcessEvent(event);
				}
			);

			seri::WindowManager::SetVSyncCount(1);
			seri::Application::SetTargetFrameRate(30);
		}

		~EditorLayer() override = default;

		void OnRender() override
		{
			editorGUI->Update();
			editorGUI->Render();
		}

	private:
		std::shared_ptr<EditorGUI> editorGUI;

	};
}
