#include "WindowManager.h"
#include "Util.h"
#include "GUI.h"
#include "Control.h"
#include "Mat4.h"
#include "Layer.h"
#include "Camera.h"
#include "Factory.h"
#include "Logger.h"
#include "State.h"
#include "Typer.h"
#include "Light.h"
#include "Skybox.h"

int main(int argc, char** argv) {
    setlocale(LC_ALL, "en_US.UTF-8");

    WindowProperties windowProperties{ /*title*/ "Maze", /*fullscreen*/ false, /*w*/ 1280, /*h*/ 720 };
    std::unique_ptr<WindowManager> windowManager = std::make_unique<WindowManager>(windowProperties);

    std::shared_ptr<State> state = std::make_shared<State>();
    state->gameState() = GameState::MENU;

    CameraProperties cameraProperties{ /*aspect*/ static_cast<float>(windowProperties.windowWidth / windowProperties.windowHeight) };
    std::shared_ptr<Camera> camera = std::make_shared<Camera>(cameraProperties, state.get());

    Control control{ windowManager.get(), camera.get(), state.get() };

    Layer layers{};

    GUI gui{ windowManager.get(), camera.get(), &layers, state.get() };

    Typer typer{ camera.get(), windowProperties.windowWidth, windowProperties.windowHeight };

    Light light{ camera.get() };

    Skybox skybox{ camera.get() };

    layers.addLayer(Factory::CreateEntity(camera.get(), { EntityType::CUBE }));
    layers.addLayer(&light);
    layers.addLayer(&skybox);

    LOGGER(info, "starting game loop");

    while (!glfwWindowShouldClose(windowManager->getWindow())) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        control.processInput(windowManager->updateDeltaTime());

        for (auto entity : layers.getLayers()) {
            entity->display();
            gui.registerEntity(entity);
        }

        typer.renderText("this is a test", windowProperties.windowWidth / 2.0f, windowProperties.windowHeight / 2.0f);

        gui.display();

        glfwPollEvents();
        glfwSwapBuffers(windowManager->getWindow());
    }

    LOGGER(info, "game loop stopped");

    return 0;
}

//#include "Game.h"
//#include "Maze.h"
//#include "Util.h"
//#include "Light.h"
//#include "Skybox.h"
//#include "CameraMaze.h"
//#include "ControlMaze.h"
//#include "WindowManager.h"
//
//int main(int argc, char** argv) {
//    WindowProperties windowProperties{ /*title*/ "Maze", /*fullscreen*/ true, /*w*/ 1280, /*h*/ 720 };
//    WindowManager windowManager(windowProperties);
//    GLFWwindow* window = windowManager.getWindow();
//    windowManager.disableCursor();
//
//    float mazeWidth = 20, mazeHeight = 20, thickness = 5.0f;
//
//    Maze* maze = new Maze{ thickness, mazeWidth, mazeHeight };
//
//    CameraProperties cameraProperties;
//    cameraProperties.position = glm::vec3(0, -thickness * 5, -thickness * 4 - mazeHeight * thickness);
//    CameraMaze* camera = new CameraMaze(cameraProperties);
//    camera->setDimensions(mazeWidth, mazeHeight, thickness);
//    camera->setWallPos(maze->getVerticalWallPosition(), maze->getHorizontalWallPosition());
//
//    Light light;
//    Skybox skybox;
//    ControlMaze control{ &windowManager, camera, &maze };
//    Game game{ static_cast<float>(windowManager.getWidth()), static_cast<float>(windowManager.getHeight()) };
//
//    bool escaping = false, restart = false;
//    while (!glfwWindowShouldClose(window)) {
//        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
//        glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
//
//        camera->handleInput(window, escaping, restart);
//        camera->update();
//
//        maze->display(escaping);
//
//        glm::mat4 view = camera->getView();
//        glm::mat4 projection = camera->getProjection();
//
//        skybox.display(view, projection);
//
//        light.setPosition(cameraProperties.position + glm::vec3(0, -10, 20));
//        light.setViewProjection(view, projection);
//        light.light();
//
//        game.display(control.getUserInput(), camera->getIsPlaying(), restart, camera->checkWin());
//
//        glfwSwapBuffers(window);
//        glfwPollEvents();
//    }
//
//    glfwTerminate();
//    return 0;
//}
