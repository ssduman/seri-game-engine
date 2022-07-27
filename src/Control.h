#pragma once

#include "Game.h"
#include "Camera.h"
#include "Window.h"
#include "IControl.h"

class Control : public IControl {
public:
    Control(Window window) : IControl(window) {
        glfwSetWindowUserPointer(_window.getWindow(), static_cast<void*>(this));
    };

    ~Control() = default;

    void registerControls() override {
        glfwSetCharCallback(_window.getWindow(),
            [](GLFWwindow* window, unsigned int c) {
                if (auto control = static_cast<Control*>(glfwGetWindowUserPointer(window))) {
                    control->charCallback(window, c);
                }
            }
        );
        glfwSetScrollCallback(_window.getWindow(),
            [](GLFWwindow* window, double x, double y) {
                if (auto control = static_cast<Control*>(glfwGetWindowUserPointer(window))) {
                    control->scrollCallback(window, x, y);
                }
            }
        );
        glfwSetCursorPosCallback(_window.getWindow(),
            [](GLFWwindow* window, double x, double y) {
                if (auto control = static_cast<Control*>(glfwGetWindowUserPointer(window))) {
                    control->cursorPosCallback(window, x, y);
                }
            }
        );
        glfwSetKeyCallback(_window.getWindow(),
            [](GLFWwindow* window, int key, int scanCode, int action, int mods) {
                if (auto control = static_cast<Control*>(glfwGetWindowUserPointer(window))) {
                    control->keyCallback(window, key, scanCode, action, mods);
                }
            }
        );
    }

    void charCallback(GLFWwindow* window, unsigned int c) override {
        userInput += c;
    }

    void scrollCallback(GLFWwindow* window, double x, double y) override {
    }

    void cursorPosCallback(GLFWwindow* window, double mouseXPos, double mouseYPos) override {
        double deltaX = mouseXPos - mouseCurrentPosX;
        double deltaY = mouseYPos - mouseCurrentPosY;

        mouseCurrentPosX = mouseXPos;
        mouseCurrentPosY = mouseYPos;
    }

    void keyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods) override {
        if ((key == GLFW_KEY_ENTER) && (action == GLFW_PRESS)) {
            userInput = "";
        } else if ((key == GLFW_KEY_BACKSPACE) && (action == GLFW_PRESS)) {
            if (userInput.size() > 0) {
                userInput.pop_back();
            }
        } else if ((key == GLFW_KEY_ESCAPE) && (action == GLFW_PRESS)) {
            exit(0);
        }
    }

private:

};
