#pragma once

#include <seri/core/Seri.h>

#include "gui/GUI.h"
#include "app/Factory.h"
#include "camera/Camera.h"
#include "event/UserEvent.h"
#include "behaviour/SimpleDrawerBehaviour.h"

#include <memory>
#include <stdexcept>

class RunnerEditor : public seri::IRunner
{
public:
	RunnerEditor() = default;

	~RunnerEditor() override = default;

	void operator()()
	{
		seri::WindowProperties windowProperties{ /*title*/ "Seri Game Engine - Editor", /*fullscreen*/ false, /*w*/ 1280, /*h*/ 720 };
		auto windowManager = seri::WindowManagerFactory::instance();
		windowManager->setWindowProperties(windowProperties);
		windowManager->init();

		seri::Graphic::Init();
		seri::BehaviourManager::Init();
		seri::Texture::SetTextureFlip(true);
		seri::ShaderManager::Init("assets/shaders/");
		seri::font::FontManager::Init("assets/fonts/");
		seri::sound::SoundManager::Init("assets/sounds/");
		//seri::sound::SoundManager::Play("jingles/Preview.wav");

		seri::SceneBuilder builder;
		auto rootScene = builder.setName("Main").build();

		windowManager->setEventCallback(seri::event::makeEventCallback(
			[&rootScene](seri::event::IEventData& data)
			{
				seri::event::EventDispatcher{}(rootScene, data);
			}
		));

		seri::CameraProperties cameraPropertiesOrtho;
		cameraPropertiesOrtho.width = windowProperties.windowWidth;
		cameraPropertiesOrtho.height = windowProperties.windowHeight;
		cameraPropertiesOrtho.isOrtho = true;
		cameraPropertiesOrtho.aspect = windowManager->getAspect();
		auto cameraOrtho = std::make_shared<Camera>(std::move(cameraPropertiesOrtho));
		cameraOrtho->init();

		seri::CameraProperties cameraPropertiesPerspective;
		cameraPropertiesPerspective.width = windowProperties.windowWidth;
		cameraPropertiesPerspective.height = windowProperties.windowHeight;
		cameraPropertiesPerspective.isOrtho = false;
		cameraPropertiesPerspective.aspect = windowManager->getAspect();
		cameraPropertiesPerspective.position = { 0.0f, 4.0f, 6.0f };
		cameraPropertiesPerspective.rotation = { -90.0f, -30.0f, 0.0f };
		auto cameraPerspective = std::make_shared<Camera>(std::move(cameraPropertiesPerspective));
		cameraPerspective->init();

		seri::Graphic::AddCamera(cameraOrtho);
		seri::Graphic::AddCamera(cameraPerspective);

		auto cameraOrthoScene = builder.setName("Camera Ortho").setObject(cameraOrtho).build();
		rootScene->add(cameraOrthoScene);

		auto cameraPerspectiveScene = builder.setName("Camera Perspective").setObject(cameraPerspective).build();
		rootScene->add(cameraPerspectiveScene);

		auto skybox = std::make_shared<seri::Skybox>(cameraPerspective);

		auto gui = std::make_shared<GUI>(cameraPerspective, rootScene);
		gui->init();

		SimpleDrawerBehaviour simpleDrawerBehaviour{};

		seri::BehaviourManager::InitBehaviours();

		LOGGER(info, "[editor] starting seri game engine - editor loop");

		while (!windowManager->windowShouldClose())
		{
			windowManager->clear();
			windowManager->clearColor();
			windowManager->updateDeltaTime();

			cameraOrtho->update();
			seri::Graphic::GetCameraPerspective()->update();

			skybox->Update();
			skybox->Render();

			seri::BehaviourManager::UpdateBehaviours();

			DrawScene(rootScene);

			gui->display();

			windowManager->pollEvents();
			windowManager->swapBuffers();
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
			s->draw();
		}
	}

};
