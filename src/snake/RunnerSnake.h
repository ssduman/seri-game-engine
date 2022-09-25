#pragma once

#include "../engine/IRunner.h"
#include "../engine/Triangle.h"

#include "Snake.h"
#include "Camera.h"
#include "Control.h"
#include "SnakeProperties.h"

class RunnerSnake : public IRunner {
public:
    RunnerSnake() = default;

    ~RunnerSnake() override = default;

    void operator()() {
        WindowProperties windowProperties{ /*title*/ "Seri Game Engine - Snake", /*fullscreen*/ false, /*w*/ 800, /*h*/ 800 };
        std::unique_ptr<WindowManager> windowManager = std::make_unique<WindowManager>(windowProperties);
        windowManager->setPointSize(20.0f);

        std::shared_ptr<State> state = std::make_shared<State>();
        state->gameState() = GameState::GAME;

        CameraProperties cameraProperties{};
        std::shared_ptr<Camera> camera = std::make_shared<Camera>(cameraProperties, state.get());
        camera->init();

        SnakeProperties snakeProperties{ /*w*/ 800.0f, /*h*/ 800.0f, /*i*/ 50.0f };
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