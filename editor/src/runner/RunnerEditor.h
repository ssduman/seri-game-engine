#pragma once

#include <seri/core/Seri.h>

#include "gui/GUI.h"
#include "camera/Camera.h"
#include "event/UserEvent.h"
#include "behaviour/BasicBehaviour.h"
#include "behaviour/SimpleBehaviour.h"

#include <ctime>
#include <memory>
#include <thread>
#include <stdexcept>

class RunnerEditor : public seri::IRunner
{
public:
	RunnerEditor() = default;

	~RunnerEditor() override = default;

	void operator()()
	{
		srand(static_cast<unsigned int>(time(0)));

		seri::WindowProperties windowProperties{ /*title*/ "Seri Game Engine - Editor", /*fullscreen*/ false, /*w*/ 1280, /*h*/ 720 };
		auto& windowManager = seri::WindowManagerFactory::Instance();
		windowManager->SetWindowProperties(windowProperties);
		windowManager->Init();

		seri::RenderingProperties renderingProperties{};
		auto& renderingManager = seri::RenderingManagerFactory::Instance();
		renderingManager->SetRenderingProperties(renderingProperties);
		renderingManager->Init(windowManager);

		seri::TimeWrapper::Init();
		seri::Application::Init();
		seri::Graphic::Init();
		seri::InputManager::Init();
		seri::BehaviourManager::Init();
		seri::ShaderLibrary::Init("assets/shaders/");
		seri::font::FontManager::Init("assets/fonts/");
		seri::sound::SoundManager::Init("assets/sounds/");

		seri::SceneBuilder builder;
		auto rootScene = builder.SetName("Main").Build();

		windowManager->AddEventCallback(seri::event::MakeEventCallback(
			[&rootScene](seri::event::IEventData& data)
			{
				seri::event::EventDispatcher{}(rootScene, data);
			}
		));

		windowManager->AddEventCallback(seri::event::MakeEventCallback(
			[&renderingManager](seri::event::IEventData& data)
			{
				if (data.eventType == seri::event::EventType::window_resize)
				{
					auto& d = seri::event::EventDispatcher::CastEventData<seri::event::WindowResizeEventData&>(data);
					renderingManager->SetViewport(0, 0, d.width, d.height);
				}
			}
		));

		seri::CameraProperties cameraPropertiesOrtho;
		cameraPropertiesOrtho.width = windowProperties.windowWidth;
		cameraPropertiesOrtho.height = windowProperties.windowHeight;
		cameraPropertiesOrtho.isOrtho = true;
		cameraPropertiesOrtho.aspect = windowManager->GetAspectRatio();
		auto cameraOrtho = std::make_shared<Camera>(std::move(cameraPropertiesOrtho));
		cameraOrtho->Init();

		seri::CameraProperties cameraPropertiesPerspective;
		cameraPropertiesPerspective.width = windowProperties.windowWidth;
		cameraPropertiesPerspective.height = windowProperties.windowHeight;
		cameraPropertiesPerspective.isOrtho = false;
		cameraPropertiesPerspective.aspect = windowManager->GetAspectRatio();
		cameraPropertiesPerspective.position = { 0.0f, 4.0f, 6.0f };
		cameraPropertiesPerspective.rotation = { -90.0f, -30.0f, 0.0f };
		auto cameraPerspective = std::make_shared<Camera>(std::move(cameraPropertiesPerspective));
		cameraPerspective->Init();

		seri::Graphic::AddCamera(cameraOrtho);
		seri::Graphic::AddCamera(cameraPerspective);

		auto cameraOrthoScene = builder.SetName("Camera Ortho").SetUnderlyingObject(cameraOrtho).Build();
		rootScene->Add(cameraOrthoScene);

		auto cameraPerspectiveScene = builder.SetName("Camera Perspective").SetUnderlyingObject(cameraPerspective).Build();
		rootScene->Add(cameraPerspectiveScene);

		auto skybox = std::make_shared<seri::Skybox>(cameraPerspective);

		auto gui = std::make_shared<GUI>(cameraPerspective, rootScene);
		gui->Init();

		windowManager->AddProcessEventDelegate(
			[&gui](const void* event)
			{
				gui->ProcessEvent(event);
			}
		);

		BasicBehaviour basicBehaviour{};
		SimpleBehaviour simpleBehaviour{};

		seri::BehaviourManager::InitBehaviours();

		seri::Application::SetVSyncCount(1);
		seri::Application::SetTargetFrameRate(60);

		LOGGER(info, "[editor] starting seri game engine - editor loop");

		while (!windowManager->GetWindowShouldClose())
		{
			seri::Application::SetFrameBegin();

			renderingManager->Clear();
			renderingManager->ClearColor();
			windowManager->UpdateDeltaTime();

			seri::Graphic::GetCameraOrtho()->Update();
			seri::Graphic::GetCameraPerspective()->Update();

			skybox->Update();
			skybox->Render();

			seri::BehaviourManager::UpdateBehaviours();

			DrawScene(rootScene);

			gui->Display();

			seri::InputManager::Reset();

			windowManager->PollEvents();
			windowManager->SwapBuffers();

			seri::Application::SetFrameEnd();

			double waitTime = seri::Application::GetWaitTime();
			if (waitTime > 0.0)
			{
				//std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(waitTime));
			}
		}

		seri::BehaviourManager::DestroyBehaviours();

		LOGGER(info, "[editor] seri game engine - editor loop stopped");
	}

private:
	void DrawScene(std::shared_ptr<seri::IScene>& rootScene)
	{
		seri::SceneIterator iter(rootScene);
		for (auto& s : iter)
		{
			s->Draw();
		}
	}

};
