#pragma once

#include "../engine/IRunner.h"

#include "Camera.h"
#include "Control.h"
#include "Fractal.h"

class RunnerFractal : public IRunner {
public:
    RunnerFractal() = default;

    ~RunnerFractal() override = default;

    void operator()() {
        WindowProperties windowProperties{ /*title*/ "Seri Game Engine", /*fullscreen*/ false, /*w*/ 600, /*h*/ 800 };
        std::unique_ptr<WindowManager> windowManager = std::make_unique<WindowManager>(windowProperties);
        windowManager->disableCursor();
        windowManager->setPointSize(2.0f);
        windowManager->setLineWidth(2.0f);

        std::shared_ptr<State> state = std::make_shared<State>();
        state->gameState() = GameState::GAME;

        CameraProperties cameraProperties{};
        cameraProperties.aspect = windowManager->getWidthF() / windowManager->getHeightF();
        cameraProperties.position = glm::vec3{ 0.0f, 10.0f, -15.0f };
        std::shared_ptr<Camera> camera = std::make_shared<Camera>(cameraProperties, state.get());

        Layer layers{};
        Control control{ windowManager.get(), camera.get(), state.get() };
        Typer typer{ camera.get(), windowProperties.windowWidth, windowProperties.windowHeight };

        auto fractal = Fractal{ camera.get(), layers };
        fractal.BarnsleyFern();

        LOGGER(info, "starting seri game engine loop");

        while (!glfwWindowShouldClose(windowManager->getWindow())) {
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            auto deltaTime = windowManager->updateDeltaTime();

            control.processInput(deltaTime);

            //fractal.BarnsleyFernAnimation(deltaTime);

            for (auto entity : layers.getLayers()) {
                entity->display();
            }

            glfwPollEvents();
            glfwSwapBuffers(windowManager->getWindow());
        }

        LOGGER(info, "seri game engine loop stopped");
    }

private:
    std::string toRender{ "this is a test" };

};
