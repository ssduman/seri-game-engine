#pragma once

#include <seri/core/Seri.h>

#include "gui/GUI.h"
#include "app/Factory.h"
#include "camera/Camera.h"
#include "event/UserEvent.h"

#include <memory>
#include <stdexcept>

class RunnerEditor : public IRunner
{
public:
	RunnerEditor() = default;

	~RunnerEditor() override = default;

	void operator()()
	{
		ShaderManager::Init("assets/shaders/");

		WindowProperties windowProperties{ /*title*/ "Seri Game Engine - Editor", /*fullscreen*/ false, /*w*/ 1280, /*h*/ 720 };
		auto windowManager = WindowManagerFactory::instance();
		windowManager->setWindowProperties(std::move(windowProperties));
		windowManager->init();

		SceneBuilder builder;
		auto rootScene = builder.setName("Main").build();

		windowManager->setEventCallback(events::makeEventCallback(
			[&rootScene](IEventData& data)
			{
				EventDispatcher{}(rootScene, data);
			}
		));

		CameraProperties cameraPropertiesOrtho;
		cameraPropertiesOrtho.isOrtho = true;
		cameraPropertiesOrtho.aspect = windowManager->getAspect();
		auto cameraOrtho = std::make_shared<Camera>(std::move(cameraPropertiesOrtho));
		cameraOrtho->init();

		CameraProperties cameraPropertiesPerspective;
		cameraPropertiesPerspective.isOrtho = false;
		cameraPropertiesPerspective.aspect = windowManager->getAspect();
		auto cameraPerspective = std::make_shared<Camera>(std::move(cameraPropertiesPerspective));
		cameraPerspective->init();

		auto cameraScene = builder.setName("Camera").setObject(cameraPerspective).build();
		rootScene->add(cameraScene);

		auto gui = std::make_shared<GUI>(cameraPerspective, rootScene);
		gui->init();

		auto graphic = std::make_shared<Graphic>();

		auto quad_2d = MeshG::quad_2d();
		auto quad_3d = MeshG::quad_3d();
		auto cube_3d = MeshG::cube_3d();
		//auto line = MeshG::line();

		auto entityShader = ShaderManager::Find("entity");

		Texture::setTextureFlip(true);
		auto passageTexture = std::make_shared<Texture>();
		passageTexture->init("assets/textures/passage.png");

		auto material = std::make_shared<MaterialG>();
		material->shader = entityShader;
		material->texture = passageTexture;
		
		LOGGER(info, "starting seri game engine - editor loop");

		while (!windowManager->windowShouldClose())
		{
			windowManager->clear();
			windowManager->clearColor();
			windowManager->updateDeltaTime();

			cameraOrtho->update();
			cameraPerspective->update();

			glm::vec3 pos_2d{ 660.0f, 660.0f, 0.0f };
			glm::quat rot_2d = glm::quat(glm::vec3{ glm::radians(0.0f), glm::radians(0.0f), glm::radians(0.0f) });
			glm::vec3 scale_2d{ 100.0f, 100.0f, 100.0f };

			glm::vec3 pos_3d{ 1.0f, 0.0f, 1.0f };
			glm::quat rot_3d = glm::quat(glm::vec3{ glm::radians(0.0f), glm::radians(0.0f), glm::radians(0.0f) });
			glm::vec3 scale_3d{ 1.0f, 1.0f, 1.0f };

			glm::vec3 pos_line{ 0.0f, 0.0f, 0.0f };
			glm::quat rot_line = glm::quat(glm::vec3{ glm::radians(0.0f), glm::radians(0.0f), glm::radians(0.0f) });
			glm::vec3 scale_line{ 10.0f, 10.0f, 10.0f };

			//graphic->draw(quad_2d, Util::GetTRS(pos_2d, rot_2d, scale_2d), material, cameraOrtho);
			graphic->draw(quad_3d, Util::GetTRS(pos_3d, rot_3d, scale_3d), material, cameraPerspective);
			graphic->draw(cube_3d, Util::GetTRS(pos_3d, rot_3d, scale_3d), material, cameraPerspective);
			//graphic->draw(line, Util::GetTRS(pos_line, rot_line, scale_line), material, cameraPerspective);

			gui->display();

			windowManager->pollEvents();
			windowManager->swapBuffers();
		}

		LOGGER(info, "seri game engine - editor loop stopped");
	}

private:
	void CreateDemoScene(std::shared_ptr<IWindowManager> windowManager, SceneBuilder builder, std::shared_ptr<IScene> rootScene, std::shared_ptr<Camera> cameraPerspective)
	{
		if (!_showDemoScene)
		{
			return;
		}

		auto circle1Scene = builder.setName("Circle1").setObject(Factory::CreateEntity(cameraPerspective, EntityType::circle)).build();
		auto triangle1Scene = builder.setName("Triangle1").setObject(Factory::CreateEntity(cameraPerspective, EntityType::triangle)).build();
		auto rectangle1Scene = builder.setName("Rectangle1").setObject(Factory::CreateEntity(cameraPerspective, EntityType::rectangle)).build();

		auto typer1 = std::make_shared<Typer>(windowManager->getWidth(), windowManager->getHeight(), "assets/fonts/En Bloc.ttf");
		typer1->getShader().init("assets/shaders/typer_vs.shader", "assets/shaders/typer_fs.shader");
		ShaderManager::GetInstance().setProjection(typer1->getShader(), glm::ortho(0.0f, windowManager->getWidthF(), 0.0f, windowManager->getHeightF()));
		typer1->init();
		typer1->setText("hello, world!");
		typer1->setColor({ 0.6f, 0.1f, 0.3f, 1.0f });
		typer1->setPosition(10.0f, 20.0f);
		auto typer1Scene = builder.setName("Typer1").setObject(typer1).build();

		std::vector<std::string> face = {
			"assets/textures/skybox/right.jpg",
			"assets/textures/skybox/left.jpg",
			"assets/textures/skybox/bottom.jpg",
			"assets/textures/skybox/top.jpg",
			"assets/textures/skybox/front.jpg",
			"assets/textures/skybox/back.jpg",
		};
		auto skybox1 = std::make_shared<Skybox>(cameraPerspective, face);
		skybox1->getShader().init("assets/shaders/skybox_vs.shader", "assets/shaders/skybox_fs.shader");
		skybox1->init();
		auto skybox1Scene = builder.setName("Skybox2").setObject(skybox1).build();

		auto model1 = std::make_shared<Model>(cameraPerspective);
		model1->getShader().init("assets/shaders/entity_vs.shader", "assets/shaders/entity_fs.shader");
		model1->init();
		model1->load("assets/models/spider.obj");
		model1->getTransform()._scale = glm::vec3{ 0.05f, 0.05f, 0.05f };
		model1->getTransform()._position = glm::vec3{ 180.0f, 20.0f, 500.0f };
		ShaderManager::GetInstance().setModel(model1->getShader(), model1->getTransform().apply());
		ShaderManager::GetInstance().SetColor(model1->getShader(), glm::vec4{ 0.1f, 0.1f, 0.1f, 1.0f });
		auto model1Scene = builder.setName("Spider1").setObject(model1).build();

		auto component1Scene = builder.setName("Component1").add(triangle1Scene).add(rectangle1Scene).build();
		auto component2Scene = builder.setName("Component2").add(circle1Scene).build();
		auto component3Scene = builder.setName("Text1").add(typer1Scene).build();
		auto component4Scene = builder.setName("Model1").add(model1Scene).build();
		auto component5Scene = builder.setName("Skybox1").add(skybox1Scene).build();

		rootScene->add(component1Scene);
		rootScene->add(component2Scene);
		rootScene->add(component3Scene);
		rootScene->add(component4Scene);
		rootScene->add(component5Scene);
	}

	void DrawScene(std::shared_ptr<IScene> rootScene)
	{
		SceneIterator iter(rootScene);
		for (auto& s : iter)
		{
			s->draw();
		}
	}

	bool _showDemoScene{ false };

};
