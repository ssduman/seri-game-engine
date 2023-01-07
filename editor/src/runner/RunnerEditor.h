#pragma once

#include <core/Seri.h>

#include "gui/GUI.h"
#include "camera/Camera.h"
#include "control/Control.h"
#include "app/Factory.h"

#include <memory>
#include <stdexcept>

class RunnerEditor : public IRunner {
public:
    RunnerEditor() = default;

    ~RunnerEditor() override = default;

    void operator()() {
        WindowProperties windowProperties{ /*title*/ "Seri Game Engine - Editor", /*fullscreen*/ false, /*w*/ 1280, /*h*/ 720 };
        auto windowManager = std::make_shared<WindowManager>(windowProperties);
        if (!windowManager->init()) {
            throw std::runtime_error("could not create window manager");
        }

        SceneBuilder builder;
        auto scene = builder.setName("Main").build();

        auto state = std::make_shared<State>();
        state->gameState() = GameState::menu;

        CameraProperties cameraProperties;
        cameraProperties.aspect = windowManager->getAspect();
        auto camera = std::make_shared<Camera>(std::move(cameraProperties), state);
        camera->init();

        if (_showDemoScene) {
            auto cameraScene = builder.setName("Camera").setObject(camera).build();
            auto circle1Scene = builder.setName("Circle1").setObject(Factory::CreateEntity(camera, EntityType::circle)).build();
            auto triangle1Scene = builder.setName("Triangle1").setObject(Factory::CreateEntity(camera, EntityType::triangle)).build();
            auto rectangle1Scene = builder.setName("Rectangle1").setObject(Factory::CreateEntity(camera, EntityType::rectangle)).build();

            auto typer1 = std::make_shared<Typer>(windowManager->getWidth(), windowManager->getHeight(), "editor-assets/fonts/En Bloc.ttf");
            typer1->getShader().init("editor-assets/shaders/typer_vs.shader", "editor-assets/shaders/typer_fs.shader");
            typer1->getShaderManager().setProjection(glm::ortho(0.0f, windowManager->getWidthF(), 0.0f, windowManager->getHeightF()));
            typer1->init();
            typer1->setText("hello, world!");
            typer1->setColor({ 0.6f, 0.1f, 0.3f, 1.0f });
            typer1->setPosition(10.0f, 20.0f);
            auto typer1Scene = builder.setName("Typer1").setObject(typer1).build();

            std::vector<std::string> face = {
                "editor-assets/textures/skybox/right.jpg",
                "editor-assets/textures/skybox/left.jpg",
                "editor-assets/textures/skybox/bottom.jpg",
                "editor-assets/textures/skybox/top.jpg",
                "editor-assets/textures/skybox/front.jpg",
                "editor-assets/textures/skybox/back.jpg",
            };
            auto skybox1 = std::make_shared<Skybox>(camera, face);
            skybox1->getShader().init("editor-assets/shaders/skybox_vs.shader", "editor-assets/shaders/skybox_fs.shader");
            skybox1->init();
            auto skybox1Scene = builder.setName("Skybox2").setObject(skybox1).build();

            auto model1 = std::make_shared<Model>(camera);
            model1->getShader().init("editor-assets/shaders/entity_vs.shader", "editor-assets/shaders/entity_fs.shader");
            model1->init();
            model1->load("editor-assets/models/spider.obj");
            model1->getTransform()._scale = glm::vec3{ 0.05f, 0.05f, 0.05f };
            model1->getTransform()._position = glm::vec3{ 180.0f, 20.0f, 500.0f };
            model1->getShaderManager().setModel(model1->getTransform().apply());
            model1->getShaderManager().setColor({ 0.1f, 0.1f, 0.1f, 1.0f });
            auto model1Scene = builder.setName("Spider1").setObject(model1).build();

            auto component1Scene = builder.setName("Component1").add(triangle1Scene).add(rectangle1Scene).build();
            auto component2Scene = builder.setName("Component2").add(circle1Scene).build();
            auto component3Scene = builder.setName("Text1").add(typer1Scene).build();
            auto component4Scene = builder.setName("Model1").add(model1Scene).build();
            auto component5Scene = builder.setName("Skybox1").add(skybox1Scene).build();

            scene->add(cameraScene);
            scene->add(component1Scene);
            scene->add(component2Scene);
            scene->add(component3Scene);
            scene->add(component4Scene);
            scene->add(component5Scene);
        }

        scene->visit(makeSceneVisitor(
            [](std::shared_ptr<IScene>& scene) {
                LOGGER(verbose, "scene id: " << scene->getId() << ", name: " << scene->getName());
            }
        ));

        Control control{ windowManager, state, camera };
        control.init();

        GUI gui{ windowManager, camera, scene, state };
        gui.init();

        LOGGER(info, "starting seri game engine - editor loop");

        while (!windowManager->windowShouldClose()) {
            windowManager->clear();
            windowManager->clearColor();

            control.processInput(windowManager->updateDeltaTime());

            //scene->draw();
            SceneIterator iter(scene);
            for (auto& s : iter) {
                s->draw();
            }

            gui.display();

            windowManager->pollEvents();
            windowManager->swapBuffers();
        }

        LOGGER(info, "seri game engine - editor loop stopped");
    }

private:
    bool _showDemoScene{ true };

};
