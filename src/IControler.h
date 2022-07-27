#pragma once

#include "Game.h"
#include "Camera.h"
#include "Window.h"

#include <string>
#include <vector>
#include <sstream>

class IControler
{
public:
    IControler() = default;

    IControler(Window window) :
        _window(&window),
        mouseCurrentPosX(window.getMouseX()),
        mouseCurrentPosY(window.getMouseY()) {
    }

    virtual ~IControler() = default;

    virtual void registerCamera(Camera* camera) = 0;

    virtual void registerGame(Game* game) = 0;

    virtual void mouseMoveCallback(GLFWwindow* window, double mouseXPos, double mouseYPos) = 0;

    virtual void mouseScrollCallback(GLFWwindow* window, double x, double y) = 0;

    virtual void charCallback(GLFWwindow* window, unsigned int c) = 0;

    virtual void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) = 0;

protected:
    Game* _game = nullptr;
    Camera* _camera = nullptr;
    Window* _window = nullptr;
    std::string userInput;
    double mouseCurrentPosX;
    double mouseCurrentPosY;
};
