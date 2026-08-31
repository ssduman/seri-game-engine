#pragma once

#include <seri/core/Seri.h>

#include "editor_layer/EditorLayer.h"
#include "editor_behaviour/BasicBehaviour.h"
#include "editor_behaviour/SimpleBehaviour.h"
#include "editor_behaviour/SystemBehaviour.h"

#include <thread>
#include <memory>
#include <vector>

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
			layerManager.AddLayer(std::make_shared<seri::editor::EditorLayer>());

			//BasicBehaviour basicBehaviour{};
			SimpleBehaviour simpleBehaviour{};
			SystemBehaviour systemBehaviour{};

			seri::BehaviourManager::InitBehaviours();

			LOGGER(info) << "[editor] seri game engine - editor loop starting";

			while (!seri::WindowManager::GetWindowShouldClose())
			{
				layerManager.PreUpdate();
				layerManager.Update();
				layerManager.PostUpdate();
			}

			seri::BehaviourManager::DestroyBehaviours();

			LOGGER(info) << "[editor] seri game engine - editor loop stopped";
		}

	};
}
