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

    virtual void charCallback(GLFWwindow* window, unsigned int codepoint) = 0;

    virtual void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) = 0;

    virtual void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) = 0;

    virtual void framebufferSizeCallback(GLFWwindow* window, int width, int height) = 0;

    virtual void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) = 0;

    virtual void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) = 0;

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
