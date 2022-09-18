#pragma once

#include "../engine/IRunner.h"

#include "GUI.h"
#include "Camera.h"
#include "Control.h"
#include "Factory.h"

class Runner : public IRunner {
public:
    Runner() = default;

    ~Runner() override = default;

    void operator()() {
        WindowProperties windowProperties{ /*title*/ "Seri Game Engine", /*fullscreen*/ false, /*w*/ 1280, /*h*/ 720 };
        std::unique_ptr<WindowManager> windowManager = std::make_unique<WindowManager>(windowProperties);

        std::shared_ptr<State> state = std::make_shared<State>();
        state->gameState() = GameState::MENU;

        CameraProperties cameraProperties{};
        cameraProperties.aspect = windowManager->getWidthF() / windowManager->getHeightF();
        std::shared_ptr<Camera> camera = std::make_shared<Camera>(cameraProperties, state.get());
        camera->init();

        Layer layers{};

        Control control{ windowManager.get(), camera.get(), state.get() };
        control.init();

        GUI gui{ windowManager.get(), camera.get(), &layers, state.get() };
        gui.init();

        layers.addLayer(Factory::CreateEntity(camera.get(), EntityType::CUBE));

        LOGGER(info, "starting seri game engine loop");

        while (!glfwWindowShouldClose(windowManager->getWindow())) {
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            control.processInput(windowManager->updateDeltaTime());

            for (auto entity : layers.getLayers()) {
                entity->display();
                gui.registerEntity(entity);
            }

            gui.display();

            glfwPollEvents();
            glfwSwapBuffers(windowManager->getWindow());
        }

        LOGGER(info, "seri game engine loop stopped");
    }

};
