#include "Window.h"
#include "GUI.h"
#include "Shader.h"
#include "Control.h"
#include "Triangle.h"
#include "Rectangle.h"

#include <locale.h>

int main(int argc, char** argv) {
    setlocale(LC_ALL, "en_US.UTF-8");

    WindowProperties windowProperties = { /*title*/ "Maze", /*fullscreen*/ false, /*w*/ 1280, /*h*/ 720 };
    Window gameWindow(windowProperties);

    Control control(gameWindow);
    control.init();

    GUI gui(gameWindow);
    gui.init();

    EntityProperties triangleProperties = {
        { glm::vec3{ 960, 180, 0 }, glm::vec3{ 320, 180, 0 }, glm::vec3{ 640, 540, 0 } },
        { glm::vec3{ 255, 0, 0 }, glm::vec3{ 0, 255, 0 }, glm::vec3{ 0, 0, 255 } },
    };
    Triangle triangle(windowProperties, triangleProperties);
    triangle.initShader("shaders/ex_vs.shader", "shaders/ex_fs.shader");
    triangle.init();

    EntityProperties rectangleProperties = {
        { glm::vec3{ 320, 180, 0 }, glm::vec3{ 320, 540, 0 }, glm::vec3{ 960, 540, 0 }, glm::vec3{ 960, 180, 0 } },
        { glm::vec3{ 255, 0, 0 }, glm::vec3{ 0, 255, 0 }, glm::vec3{ 0, 0, 255 }, glm::vec3{ 255, 255, 255 } },
    };
    Rectangle rectangle(windowProperties, rectangleProperties);
    rectangle.initShader("shaders/ex_vs.shader", "shaders/ex_fs.shader");
    rectangle.init();

    while (!glfwWindowShouldClose(gameWindow.getWindow())) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        triangle.update();
        triangle.render();

        rectangle.update();
        rectangle.render();

        gui.update();
        gui.render();

        glfwPollEvents();
        glfwSwapBuffers(gameWindow.getWindow());
    }

    glfwDestroyWindow(gameWindow.getWindow());
    glfwTerminate();

    return 0;
}

//#include "Game.h"
//#include "Maze.h"
//#include "Light.h"
//#include "Camera.h"
//#include "Skybox.h"
//#include "Window.h"
//
//#include <GL/glew.h>
//#include <GLFW/glfw3.h>
//#include <glm/glm.hpp>
//
//double mouseCurrentPosX, mouseCurrentPosY;
//
//Camera* camera;
//glm::vec3 cameraPosition;
//
//Maze* maze;
//float width, height, aspect;
//float mazeWidth = 20, mazeHeight = 20, thickness = 5.0f; // cubes will be 10.0f x 10.0f x 10.0f
//
//Light* light;
//Game* game;
//std::string userInput = "";
//bool play = false;
//
//void mouseMoveCallback(GLFWwindow* window, double mouseXPos, double mouseYPos);
//void mouseScrollCallback(GLFWwindow* window, double x, double y);
//void charCallback(GLFWwindow* window, unsigned int c);
//void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
//bool win();
//
//int main(int argc, char** argv) {
//    const char* title = "Maze";
//    bool fullscreen = true;
//    int windowWidth = 1280;
//    int windowHeight = 720;
//    WindowProperties windowProperties = { /*title*/ "Maze", /*fullscreen*/ false, /*w*/ 1280, /*h*/ 720 };
//    Window thisWindow(windowProperties);
//    GLFWwindow* window = thisWindow.getWindow();
//
//    width = (float)thisWindow.getWidth();
//    height = (float)thisWindow.getHeight();
//
//    mouseCurrentPosX = thisWindow.getMouseX();
//    mouseCurrentPosY = thisWindow.getMouseY();
//
//    glfwSetCursorPosCallback(window, mouseMoveCallback);
//    glfwSetScrollCallback(window, mouseScrollCallback);
//    glfwSetCharCallback(window, charCallback);
//    glfwSetKeyCallback(window, keyCallback);
//
//    maze = new Maze(thickness, mazeWidth, mazeHeight);
//
//    cameraPosition = glm::vec3(0, -thickness * 5, -thickness * 4 - mazeHeight * thickness);
//    camera = new Camera(cameraPosition, ((float)width) / height, play);
//    camera->setDimensions(mazeWidth, mazeHeight, thickness);
//    camera->setWallPos(maze->getVerticalWallPosition(), maze->getHorizontalWallPosition());
//
//    Skybox* skybox = new Skybox();
//    light = new Light();
//    game = new Game(width, height);
//
//    bool escaping = false, restart = false;
//    while (!glfwWindowShouldClose(window)) {
//        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
//        glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
//
//        camera->keyboardControl(window, &maze, play, escaping, restart);
//        camera->update(play);
//
//        maze->display(escaping);
//
//        glm::mat4 view = camera->getView();
//        glm::mat4 projection = camera->getProjection();
//        skybox->display(view, projection);
//        light->light(cameraPosition, view, projection);
//
//        game->display(userInput, play, restart, win());
//
//        glfwSwapBuffers(window);
//        glfwPollEvents();
//    }
//
//    glfwTerminate();
//    return 0;
//}
//
//void mouseMoveCallback(GLFWwindow* window, double mouseXPos, double mouseYPos) {
//    double deltaX = mouseXPos - mouseCurrentPosX;
//    double deltaY = mouseYPos - mouseCurrentPosY;
//
//    camera->mouseControl(window, deltaX, deltaY);
//
//    mouseCurrentPosX = mouseXPos;
//    mouseCurrentPosY = mouseYPos;
//}
//
//void mouseScrollCallback(GLFWwindow* window, double x, double y) {
//    float curr = camera->getAmbient();
//    if ((y > 0) && (curr <= 0.98f)) {
//        camera->adjustAmbient(0.02f);
//    } else if ((y < 0) && (curr >= 0.02f)) {
//        camera->adjustAmbient(-0.02f);
//    }
//}
//
//void charCallback(GLFWwindow* window, unsigned int c) {
//    userInput += c;
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
//        play = true;
//
//        delete maze, camera, game;
//
//        maze = new Maze(thickness, mazeWidth, mazeHeight);
//
//        cameraPosition = glm::vec3((mazeWidth - 1) * thickness * 2, -thickness / 2, -thickness * 4); // enterence
//        camera = new Camera(cameraPosition, ((float)width) / height, play);
//        camera->setDimensions(mazeWidth, mazeHeight, thickness);
//        camera->setWallPos(maze->getVerticalWallPosition(), maze->getHorizontalWallPosition());
//    } else if ((key == GLFW_KEY_BACKSPACE) && (action == GLFW_PRESS)) {
//        if (userInput.size() > 0) {
//            userInput.pop_back();
//        }
//    }
//}
//
//bool win() {
//    glm::vec3 pos = camera->getCameraPos();
//    float t1 = thickness, t2 = thickness * 2, h = mazeHeight;
//    bool x = pos.x >= -thickness && pos.x <= thickness;
//    bool z = pos.z >= (h - 2) * t2 + t1 && pos.z <= (h - 1) * t2 + t1;
//    if (x && z) return true;
//    return false;
//}
