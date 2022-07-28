#pragma once
#pragma warning(disable: 4100)

#include "Game.h"
#include "Camera.h"
#include "Window.h"
#include "IControl.h"

class Control : public IControl {
public:
    Control(Window window) : IControl(window) {
        glfwSetWindowUserPointer(_window.getWindow(), static_cast<void*>(this));
    }

    ~Control() = default;

    void registerControls() override {
        glfwSetCharCallback(_window.getWindow(),
            [](GLFWwindow* window, unsigned int codepoint) {
                if (auto control = static_cast<Control*>(glfwGetWindowUserPointer(window))) {
                    control->charCallback(window, codepoint);
                }
            }
        );

        glfwSetMouseButtonCallback(_window.getWindow(),
            [](GLFWwindow* window, int button, int action, int mods) {
                if (auto control = static_cast<Control*>(glfwGetWindowUserPointer(window))) {
                    control->mouseButtonCallback(window, button, action, mods);
                }
            }
        );

        glfwSetScrollCallback(_window.getWindow(),
            [](GLFWwindow* window, double xoffset, double yoffset) {
                if (auto control = static_cast<Control*>(glfwGetWindowUserPointer(window))) {
                    control->scrollCallback(window, xoffset, yoffset);
                }
            }
        );

        glfwSetFramebufferSizeCallback(_window.getWindow(),
            [](GLFWwindow* window, int width, int height) {
                if (auto control = static_cast<Control*>(glfwGetWindowUserPointer(window))) {
                    control->framebufferSizeCallback(window, width, height);
                }
            }
        );

        glfwSetCursorPosCallback(_window.getWindow(),
            [](GLFWwindow* window, double xpos, double ypos) {
                if (auto control = static_cast<Control*>(glfwGetWindowUserPointer(window))) {
                    control->cursorPosCallback(window, xpos, ypos);
                }
            }
        );

        glfwSetKeyCallback(_window.getWindow(),
            [](GLFWwindow* window, int key, int scancode, int action, int mods) {
                if (auto control = static_cast<Control*>(glfwGetWindowUserPointer(window))) {
                    control->keyCallback(window, key, scancode, action, mods);
                }
            }
        );
    }

    void charCallback(GLFWwindow* window, unsigned int codepoint) override {
        userInput += (unsigned char)codepoint; // TODO: support for unicode
    }

    void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) override {
        //double deltaX = xpos - mouseCurrentPosX;
        //double deltaY = ypos - mouseCurrentPosY;

        mouseCurrentPosX = xpos;
        mouseCurrentPosY = ypos;
    }

    void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) override {
    }

    void framebufferSizeCallback(GLFWwindow* window, int width, int height) override {
        glViewport(0, 0, width, height);
    }

    void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) override {
        if ((button == GLFW_MOUSE_BUTTON_LEFT) && (action == GLFW_PRESS)) {
        }
    }

    void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) override {
        if ((key == GLFW_KEY_ENTER) && (action == GLFW_PRESS)) {
            userInput = "";
        } else if ((key == GLFW_KEY_BACKSPACE) && (action == GLFW_PRESS)) {
            if (userInput.size() > 0) {
                userInput.pop_back();
            }
        } else if ((key == GLFW_KEY_ESCAPE) && (action == GLFW_PRESS)) {
            exit(0);
        }

        if (action == GLFW_RELEASE) {
            std::cout << "userInput: " << getUserInput() << std::endl;
        }
    }

private:

};
