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
//#include "WindowManager.h"
//
//Maze* maze;
//CameraMaze* camera;
//std::string userInput = "";
//
//float mazeWidth = 20, mazeHeight = 20, thickness = 5.0f; // cubes will be 10.0f x 10.0f x 10.0f
//
//void mouseMoveCallback(GLFWwindow* window, double xpos, double ypos) {
//    float xPos = static_cast<float>(xpos);
//    float yPos = static_cast<float>(ypos);
//    camera->handleMouse(xPos, yPos);
//}
//
//void mouseScrollCallback(GLFWwindow* window, double x, double y) {
//    float curr = camera->getAmbient();
//    if ((y > 0) && (curr <= 0.98f)) {
//        camera->getAmbient() += 0.02f;
//    } else if ((y < 0) && (curr >= 0.02f)) {
//        camera->getAmbient() -= 0.02f;
//    }
//}
//
//void charCallback(GLFWwindow* window, unsigned int c) {
//    userInput += static_cast<char>(c);
//}
//
//void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
//    if ((key == GLFW_KEY_ENTER) && (action == GLFW_PRESS)) {
//        if (userInput.size() == 0) return;
//        std::stringstream userInputStream(userInput);
//        std::string part;
//        std::vector<std::string> parts;
//
//        while (std::getline(userInputStream, part, 'x')) {
//            parts.push_back(part);
//        }
//        if (userInput == part) return;
//        if (parts[0].size() == 0) return;
//        if (parts.size() != 2) return;
//
//        std::stringstream width_s(parts[0]);
//        std::stringstream height_s(parts[1]);
//        width_s >> mazeWidth;
//        height_s >> mazeHeight;
//        if ((mazeWidth == 0) || (mazeHeight == 0)) return;
//
//        delete maze, camera;
//        maze = new Maze(thickness, mazeWidth, mazeHeight);
//
//        CameraProperties cameraProperties;
//        cameraProperties.position = glm::vec3((mazeWidth - 1) * thickness * 2, -thickness / 2, -thickness * 4); // enterence
//        camera = new CameraMaze(cameraProperties);
//        camera->getIsPlaying() = true;
//        camera->setDimensions(mazeWidth, mazeHeight, thickness);
//        camera->setWallPos(maze->getVerticalWallPosition(), maze->getHorizontalWallPosition());
//    } else if ((key == GLFW_KEY_BACKSPACE) && (action == GLFW_PRESS)) {
//        if (userInput.size() > 0) {
//            userInput.pop_back();
//        }
//    }
//}
//
//int main(int argc, char** argv) {
//    WindowProperties windowProperties{ /*title*/ "Maze", /*fullscreen*/ true, /*w*/ 1280, /*h*/ 720 };
//    WindowManager windowManager(windowProperties);
//    GLFWwindow* window = windowManager.getWindow();
//
//    glfwSetKeyCallback(window, keyCallback);
//    glfwSetCharCallback(window, charCallback);
//    glfwSetScrollCallback(window, mouseScrollCallback);
//    glfwSetCursorPosCallback(window, mouseMoveCallback);
//
//    maze = new Maze(thickness, mazeWidth, mazeHeight);
//    Light* light = new Light();
//    Skybox* skybox = new Skybox();
//    Game* game = new Game(static_cast<float>(windowManager.getWidth()), static_cast<float>(windowManager.getHeight()));
//
//    CameraProperties cameraProperties;
//    cameraProperties.position = glm::vec3(0, -thickness * 5, -thickness * 4 - mazeHeight * thickness);
//    camera = new CameraMaze(cameraProperties);
//    camera->setDimensions(mazeWidth, mazeHeight, thickness);
//    camera->setWallPos(maze->getVerticalWallPosition(), maze->getHorizontalWallPosition());
//
//    bool escaping = false, restart = false;
//    while (!glfwWindowShouldClose(window)) {
//        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
//        glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
//
//        camera->handleInput(window, &maze, escaping, restart);
//        camera->update();
//
//        maze->display(escaping);
//
//        glm::mat4 view = camera->getView();
//        glm::mat4 projection = camera->getProjection();
//        skybox->display(view, projection);
//        light->setPosition(cameraProperties.position + glm::vec3(0, -10, 20));
//        light->setViewProjection(view, projection);
//        light->light();
//
//        game->display(userInput, camera->getIsPlaying(), restart, camera->checkWin());
//
//        glfwSwapBuffers(window);
//        glfwPollEvents();
//    }
//
//    glfwTerminate();
//    return 0;
//}
