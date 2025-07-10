#pragma once

#include "../engine/IRunner.h"

#include "Camera.h"
#include "Tetris.h"
#include "Control.h"
#include "TetrisProperties.h"

class RunnerTetris : public IRunner {
public:
    RunnerTetris() = default;

    ~RunnerTetris() override = default;

    void operator()() {
        WindowProperties windowProperties{ /*title*/ "Seri Game Engine - Tetris", /*fullscreen*/ false, /*w*/ 600, /*h*/ 800 };
        std::unique_ptr<WindowManager> windowManager = std::make_unique<WindowManager>(windowProperties);

        std::shared_ptr<State> state = std::make_shared<State>();
        state->gameState() = GameState::game;

        CameraProperties cameraProperties{};
        cameraProperties.width = windowManager->getWidthF();
        cameraProperties.height = windowManager->getHeightF();
        std::shared_ptr<Camera> camera = std::make_shared<Camera>(cameraProperties, state.get());
        camera->init();

        TetrisProperties tetrisProperties{ windowManager->getWidthF(), windowManager->getHeightF(), /*speed*/ 8.0f, /*interval*/ 50.0f };
        Tetris tetris{ camera.get(), tetrisProperties };
        tetris.init();

        Control control{ windowManager.get(), camera.get(), state.get(), tetris };
        control.init();

        Layer layers{};
        layers.addLayer(&tetris);

        LOGGER(info, "starting tetris loop");

        while (!glfwWindowShouldClose(windowManager->getWindow())) {
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            tetris.update(windowManager->updateDeltaTime());

            for (auto entity : layers.getLayers()) {
                entity->display();
            }

            glfwPollEvents();
            glfwSwapBuffers(windowManager->getWindow());
        }

        LOGGER(info, "tetris loop stopped");
    }

};
