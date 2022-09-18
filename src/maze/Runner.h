#pragma once

#include "../engine/IRunner.h"
#include "../engine/Logger.h"

#include "Game.h"
#include "Maze.h"
#include "Camera.h"
#include "Control.h"

class Runner : public IRunner {
public:
    Runner() = default;

    ~Runner() override = default;

    void operator()() {
        WindowProperties windowProperties{ /*title*/ "Maze", /*fullscreen*/ true };
        WindowManager windowManager{ windowProperties };
        windowManager.disableCursor();

        CameraProperties cameraProperties;
        cameraProperties.position = glm::vec3{ 0, _mazeThickness * 5, -_mazeThickness * 4 - _mazeHeight * _mazeThickness };
        std::shared_ptr<Camera> camera = std::make_shared<Camera>(cameraProperties);
        camera->initShader("maze-assets/shaders/basic_vs.shader", "maze-assets/shaders/basic_fs.shader");
        camera->setLight();
        camera->init();

        Maze* maze = new Maze{ camera.get(), _mazeWidth, _mazeHeight, _mazeThickness };
        camera->setMazeDimensions(_mazeWidth, _mazeHeight, _mazeThickness);
        camera->setMazeWallPositions(maze->getVerticalWallPosition(), maze->getHorizontalWallPosition());

        Layer layers{};

        Light light{ camera.get() };
        light.initShader("maze-assets/shaders/entity_vs.shader", "maze-assets/shaders/entity_fs.shader");
        light.initMVP();
        light.setPosition({ 0.0f, 10.0f, 8.0f });
        light.setDefaultPositions();
        light.setColor({ 1.0f, 1.0f, 1.0f, 1.0f });
        light.init();

        Skybox skybox{ camera.get() };
        skybox.initShader("maze-assets/shaders/skybox_vs.shader", "maze-assets/shaders/skybox_fs.shader");
        skybox.initMVP();
        skybox.setDefaultPositions();
        skybox.setFaces({
            "maze-assets/textures/skybox/right.jpg",
            "maze-assets/textures/skybox/left.jpg",
            "maze-assets/textures/skybox/bottom.jpg",
            "maze-assets/textures/skybox/top.jpg",
            "maze-assets/textures/skybox/front.jpg",
            "maze-assets/textures/skybox/back.jpg"
        });
        skybox.loadCubemap();
        skybox.init();

        Control control{ &windowManager, camera.get(), &maze };
        control.init();

        Game game{ camera.get(), control, windowManager.getWidthF(), windowManager.getHeightF() };

        layers.addLayer(&game);
        layers.addLayer(&light);
        layers.addLayer(&skybox);

        LOGGER(info, "starting maze game loop");

        while (!glfwWindowShouldClose(windowManager.getWindow())) {
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            camera->handleInput(windowManager.getWindow());
            camera->update();

            maze->display();

            for (auto entity : layers.getLayers()) {
                entity->display();
            }

            glfwPollEvents();
            glfwSwapBuffers(windowManager.getWindow());
        }

        LOGGER(info, "maze game loop stopped");
    }

private:
    float _mazeWidth{ 20.0f }, _mazeHeight{ 20.0f }, _mazeThickness{ 5.0f };

};
