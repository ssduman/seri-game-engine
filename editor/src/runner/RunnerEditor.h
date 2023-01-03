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

        auto scene = std::make_shared<SceneComponent>("Main");

        auto state = std::make_shared<State>();
        state->gameState() = GameState::menu;

        CameraProperties cameraProperties;
        cameraProperties.aspect = windowManager->getAspect();
        auto camera = std::make_shared<Camera>(std::move(cameraProperties), state);
        camera->init();

        if (_showDemoScene) {
            auto cameraScene = std::make_shared<SceneComponent>("Camera");
            cameraScene->setObject(camera);
            scene->add(cameraScene);

            auto component1Scene = std::make_shared<SceneComponent>("Components1");
            auto component2Scene = std::make_shared<SceneComponent>("Components2");

            auto triangle1Scene = std::make_shared<SceneComponent>("Triangle1");
            triangle1Scene->setObject(Factory::CreateEntity(camera, EntityType::triangle));
            component1Scene->add(triangle1Scene);

            auto rectangle1Scene = std::make_shared<SceneComponent>("Rectangle1");
            rectangle1Scene->setObject(Factory::CreateEntity(camera, EntityType::rectangle));
            component1Scene->add(rectangle1Scene);

            scene->add(component1Scene);
            scene->add(component2Scene);
        }

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
