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

            auto component1Scene = builder.setName("Component1").add(triangle1Scene).add(rectangle1Scene).build();
            auto component2Scene = builder.setName("Component2").add(circle1Scene).build();

            scene->add(cameraScene);
            scene->add(component1Scene);
            scene->add(component2Scene);
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

            scene->draw();

            gui.display();

            windowManager->pollEvents();
            windowManager->swapBuffers();
        }

        LOGGER(info, "seri game engine - editor loop stopped");
    }

private:
    bool _showDemoScene{ true };

};
