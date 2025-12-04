#pragma once

#include <seri/core/Seri.h>

#include "gui/GUI.h"

namespace seri::editor
{
	class EditorLayer : public seri::LayerBase
	{
	public:
		EditorLayer() : LayerBase("GUILayer")
		{
			gui = std::make_shared<GUI>();
			gui->Init();

			seri::WindowManager::Instance()->AddProcessEventDelegate(
				[&](const void* event)
				{
					gui->ProcessEvent(event);
				}
			);
		}

		~EditorLayer() override = default;

		void PreUpdate() override
		{
		}

		void Update() override
		{
			gui->Update();
			gui->Render();
		}

		void PostUpdate() override
		{
		}

	private:
		std::shared_ptr<GUI> gui;

	};
}
