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
        WindowProperties windowProperties{ /*title*/ "Maze", /*fullscreen*/ true, /*w*/ 1280, /*h*/ 720 };
        WindowManager windowManager{ windowProperties };
        windowManager.disableCursor();

        float mazeWidth{ 20.0f }, mazeHeight{ 20.0f }, thickness{ 5.0f };

        Maze* maze = new Maze{ thickness, mazeWidth, mazeHeight };

        CameraProperties cameraProperties;
        cameraProperties.position = glm::vec3{ 0, -thickness * 5, -thickness * 4 - mazeHeight * thickness };
        CameraMaze* camera = new CameraMaze{ cameraProperties };
        camera->setDimensions(mazeWidth, mazeHeight, thickness);
        camera->setWallPos(maze->getVerticalWallPosition(), maze->getHorizontalWallPosition());

        Light light;
        Skybox skybox;
        ControlMaze control{ &windowManager, camera, &maze };
        Game game{ static_cast<float>(windowManager.getWidth()), static_cast<float>(windowManager.getHeight()) };

        bool showEscapePath = false, restart = false;
        while (!glfwWindowShouldClose(windowManager.getWindow())) {
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            camera->handleInput(windowManager.getWindow(), showEscapePath, restart);
            camera->update();

            maze->display(showEscapePath);

            const glm::mat4& view = camera->getView();
            const glm::mat4& projection = camera->getProjection();

            skybox.display(view, projection);

            light.setPosition(cameraProperties.position + glm::vec3{ 0.0f, -10.0f, 20.0f });
            light.setViewProjection(view, projection);
            light.light();

            game.display(control.getUserInput(), camera->getIsPlaying(), restart, camera->checkWin());

            glfwPollEvents();
            glfwSwapBuffers(windowManager.getWindow());
        }
    }

private:

};
