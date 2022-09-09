#pragma once

#include "Game.h"
#include "Maze.h"
#include "IRunner.h"
#include "CameraMaze.h"
#include "ControlMaze.h"

class RunnerMaze : public IRunner {
public:
    RunnerMaze() {}

    virtual ~RunnerMaze() {}

    void operator()() {
        WindowProperties windowProperties{ /*title*/ "Maze", /*fullscreen*/ true };
        WindowManager windowManager{ windowProperties };
        windowManager.disableCursor();

        float mazeWidth{ 20.0f }, mazeHeight{ 20.0f }, mazeThickness{ 5.0f };

        CameraProperties cameraProperties;
        cameraProperties.position = glm::vec3{ 0, mazeThickness * 5, -mazeThickness * 4 - mazeHeight * mazeThickness };
        std::unique_ptr<CameraMaze> camera = std::make_unique<CameraMaze>(cameraProperties);

        Maze* maze = new Maze{ camera.get(), mazeWidth, mazeHeight, mazeThickness };
        camera->setMazeDimensions(mazeWidth, mazeHeight, mazeThickness);
        camera->setMazeWallPositions(maze->getVerticalWallPosition(), maze->getHorizontalWallPosition());

        Layer layers{};
        Light light{ camera.get() };
        Skybox skybox{ camera.get() };
        ControlMaze control{ &windowManager, camera.get(), &maze };
        Game game{ camera.get(), windowManager.getWidthF(), windowManager.getHeightF() };

        //layers.addLayer(maze);
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

            game.display(control.getUserInput(), camera->getIsPlaying(), camera->isRestartTriggered(), camera->checkWin());

            glfwPollEvents();
            glfwSwapBuffers(windowManager.getWindow());
        }

        LOGGER(info, "maze game loop stopped");
    }

private:

};
