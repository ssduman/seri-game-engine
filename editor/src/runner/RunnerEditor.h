#pragma once

#include <seri/core/Seri.h>

#include "layer/GUILayer.h"
#include "behaviour/BasicBehaviour.h"
#include "behaviour/SimpleBehaviour.h"

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
			layerManager.AddLayer(std::make_shared<seri::CoreLayer>());
			layerManager.AddLayer(std::make_shared<seri::editor::GUILayer>());

			BasicBehaviour basicBehaviour{};
			SimpleBehaviour simpleBehaviour{};

			seri::BehaviourManager::InitBehaviours();

			LOGGER(info, "[editor] starting seri game engine - editor loop");

			while (!seri::WindowManager::GetWindowShouldClose())
			{
				layerManager.PreUpdate();
				layerManager.Update();
				layerManager.PostUpdate();
			}

			seri::BehaviourManager::DestroyBehaviours();

			LOGGER(info, "[editor] seri game engine - editor loop stopped");
		}

	};
}
