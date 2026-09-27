#pragma once

#include "seri/layer/Layer.h"
#include "seri/core/Application.h"
#include "seri/scene/SceneManager.h"
#include "seri/window/WindowManager.h"
#include "seri/rendering/render/RenderingManager.h"

namespace seri
{
	class PlayerLayer : public LayerBase
	{
	public:
		PlayerLayer() : LayerBase("PlayerLayer")
		{
			Application::SetVSyncCount(1);

			RenderingManager::SetEditorViewVisible(false);
			RenderingManager::GetGameRT()->Resize(WindowManager::GetWidth(), WindowManager::GetHeight());

			scene::SceneManager::SetState(scene::SceneState::play);
		}

		~PlayerLayer() override = default;

		void OnUpdate() override
		{
			if (scene::SceneManager::GetState() == scene::SceneState::edit)
			{
				WindowManager::SetWindowShouldCloseToTrue();
			}
		}

		void OnRender() override
		{
			int width = WindowManager::GetWidth();
			int height = WindowManager::GetHeight();
			if (width <= 0 || height <= 0)
			{
				return;
			}

			auto gameRT = RenderingManager::GetGameRT();
			gameRT->BlitColorToScreen(width, height);
			gameRT->Resize(width, height);
		}

	};
}
