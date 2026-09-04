#pragma once

#include <seri/core/Seri.h>

#include "layer/EditorLayer.h"
#include "layer/SandboxLayer.h"

namespace seri::editor
{
	class RunnerEditor : public seri::IRunner
	{
	public:
		RunnerEditor() = default;

		~RunnerEditor() override = default;

		void operator()(int argc, char* argv[])
		{
			seri::LayerManager layerManager{};
			layerManager.AddLayer(std::move(std::make_shared<seri::CoreLayer>()));
			layerManager.AddLayer(std::move(std::make_shared<seri::editor::SandboxLayer>()));
			layerManager.AddLayer(std::move(std::make_shared<seri::editor::EditorLayer>()));

			LOGGER(info) << "[editor] seri game engine - editor loop starting";

			while (!seri::WindowManager::GetWindowShouldClose())
			{
				layerManager.OnPreUpdate();
				layerManager.OnUpdate();
				layerManager.OnRender();
				layerManager.OnPostUpdate();
			}

			LOGGER(info) << "[editor] seri game engine - editor loop stopped";
		}

	};
}
