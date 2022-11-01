#pragma once

#include "../engine/IRunner.h"
#include "../engine/Triangle.h"

#include "Snake.h"
#include "Camera.h"
#include "Control.h"

class RunnerSnake : public IRunner {
public:
    RunnerSnake() = default;

    ~RunnerSnake() override = default;

    void operator()() {
        WindowProperties windowProperties{ /*title*/ "Seri Game Engine - Snake", /*fullscreen*/ false, /*w*/ 800, /*h*/ 600 };
        std::unique_ptr<WindowManager> windowManager = std::make_unique<WindowManager>(windowProperties);

        std::shared_ptr<State> state = std::make_shared<State>();
        state->gameState() = GameState::game;

        CameraProperties cameraProperties{};
        cameraProperties.width = windowManager->getWidthF();
        cameraProperties.height = windowManager->getHeightF();
        std::shared_ptr<Camera> camera = std::make_shared<Camera>(cameraProperties, state.get());
        camera->init();

        SnakeProperties snakeProperties{ windowManager->getWidthF(), windowManager->getHeightF(), /*speed*/ 4.0f, /*interval*/ 20.0f };
        Snake snake{ camera.get(), snakeProperties };
        snake.init();

        Control control{ windowManager.get(), camera.get(), &snake, state.get() };
        control.init();

        Layer layers{};
        layers.addLayer(&snake);

        LOGGER(info, "starting snake loop");

        while (!glfwWindowShouldClose(windowManager->getWindow())) {
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            auto deltaTime = windowManager->updateDeltaTime();

            control.processInput(deltaTime);

            snake.handleTime(deltaTime);

            for (auto entity : layers.getLayers()) {
                entity->display();
            }

            glfwPollEvents();
            glfwSwapBuffers(windowManager->getWindow());
        }

        LOGGER(info, "snake loop stopped");
    }

};
