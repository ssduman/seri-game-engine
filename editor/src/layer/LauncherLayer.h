#pragma once

#include <seri/core/Seri.h>

#include "gui/launcher/LauncherGUI.h"

namespace seri::editor
{
	class LauncherLayer : public seri::LayerBase
	{
	public:
		LauncherLayer() : LayerBase("LauncherLayer")
		{
			launcherGUI = std::make_shared<LauncherGUI>();
			launcherGUI->Init();

			seri::Application::SetVSyncCount(1);
		}

		~LauncherLayer() override = default;

		void OnPreUpdate() override
		{
			seri::RenderingManager::ClearColor(0.07f, 0.08f, 0.09f, 1.0f);
			seri::RenderingManager::Clear();
		}

		void OnRender() override
		{
			launcherGUI->Update();
			launcherGUI->Render();
		}

		void OnPostUpdate() override
		{
			seri::InputManager::Reset();

			seri::WindowManager::PollEvents();
			seri::WindowManager::SwapBuffers();
		}

		const std::filesystem::path& GetProjectPath() const
		{
			return launcherGUI->GetProjectPath();
		}

	private:
		std::shared_ptr<LauncherGUI> launcherGUI;

	};
}
