#pragma once

#include "Window.h"

#include <string>

class IControl {
public:
    IControl() = default;

    IControl(Window window) :
        _window(window),
        mouseCurrentPosX(_window.getMouseX()),
        mouseCurrentPosY(_window.getMouseY()) {
    }

    virtual ~IControl() = default;

    virtual void registerControls() = 0;

    virtual void charCallback(GLFWwindow* window, unsigned int c) = 0;

    virtual void scrollCallback(GLFWwindow* window, double x, double y) = 0;

    virtual void cursorPosCallback(GLFWwindow* window, double mouseXPos, double mouseYPos) = 0;

    virtual void keyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods) = 0;

    inline std::string getUserInput() {
        return userInput;
    }

    inline double getMouseCurrentPosX() {
        return mouseCurrentPosX;
    }

    inline double getMouseCurrentPosY() {
        return mouseCurrentPosY;
    }

protected:
    Window _window;
    std::string userInput;
    double mouseCurrentPosX;
    double mouseCurrentPosY;
};
