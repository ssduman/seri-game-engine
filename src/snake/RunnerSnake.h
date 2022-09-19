#pragma once

#include "../engine/IRunner.h"
#include "../engine/Triangle.h"

#include "Camera.h"
#include "Control.h"

class RunnerSnake : public IRunner {
public:
    RunnerSnake() = default;

    ~RunnerSnake() override = default;

    void operator()() {
        WindowProperties windowProperties{ /*title*/ "Seri Game Engine - Snake", /*fullscreen*/ false, /*w*/ 800, /*h*/ 800 };
        std::unique_ptr<WindowManager> windowManager = std::make_unique<WindowManager>(windowProperties);

        std::shared_ptr<State> state = std::make_shared<State>();
        state->gameState() = GameState::GAME;

        CameraProperties cameraProperties{};
        std::shared_ptr<Camera> camera = std::make_shared<Camera>(cameraProperties, state.get());
        camera->init();

        Control control{ windowManager.get(), camera.get(), state.get() };
        control.init();

        std::vector<glm::vec2> positions{
            { 200.0f, 200.0f },
            { 600.0f, 200.0f },
            { 400.0f, 600.0f },
        };
        Triangle* triangle = new Triangle(camera.get());
        triangle->initShader("snake-assets/shaders/snake_vs.shader", "snake-assets/shaders/snake_fs.shader");
        triangle->initMVP();
        triangle->setPositionsVec2(positions);
        triangle->setColor({ 0.2f, 0.2f, 0.2f, 1.0f });
        triangle->init();

        Layer layers{};
        layers.addLayer(triangle);

        LOGGER(info, "starting snake loop");

        while (!glfwWindowShouldClose(windowManager->getWindow())) {
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            auto deltaTime = windowManager->updateDeltaTime();

            control.processInput(deltaTime);

            for (auto entity : layers.getLayers()) {
                entity->display();
            }

            glfwPollEvents();
            glfwSwapBuffers(windowManager->getWindow());
        }

        LOGGER(info, "mics snake stopped");
    }

};
