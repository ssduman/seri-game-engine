#pragma once
#pragma warning(disable: 4100)
#pragma warning(disable: 4244)

#include "../engine/IControl.h"
#include "../engine/InputHandler.h"
#include "../engine/WindowManager.h"

#include "Snake.h"
#include "Camera.h"

class Control : public IControl {
public:
    Control(WindowManager* windowManager, Camera* camera, Snake* snake, State* state)
        : IControl(windowManager, state), _camera(camera), _snake(snake), _inputHandler(camera) {
        glfwSetWindowUserPointer(_windowManager->getWindow(), static_cast<void*>(this));

        LOGGER(info, "control init succeeded");
    }

    ~Control() override {
        LOGGER(info, "control delete succeeded");
    }

    void init() override {
        glfwSetCharCallback(_windowManager->getWindow(),
            [](GLFWwindow* window, unsigned int codepoint) {
                if (auto control = static_cast<Control*>(glfwGetWindowUserPointer(window))) {
                    control->charCallback(window, codepoint);
                }
            }
        );

        glfwSetMouseButtonCallback(_windowManager->getWindow(),
            [](GLFWwindow* window, int button, int action, int mods) {
                if (auto control = static_cast<Control*>(glfwGetWindowUserPointer(window))) {
                    control->mouseButtonCallback(window, button, action, mods);
                }
            }
        );

        glfwSetScrollCallback(_windowManager->getWindow(),
            [](GLFWwindow* window, double xoffset, double yoffset) {
                if (auto control = static_cast<Control*>(glfwGetWindowUserPointer(window))) {
                    control->scrollCallback(window, xoffset, yoffset);
                }
            }
        );

        glfwSetFramebufferSizeCallback(_windowManager->getWindow(),
            [](GLFWwindow* window, int width, int height) {
                if (auto control = static_cast<Control*>(glfwGetWindowUserPointer(window))) {
                    control->framebufferSizeCallback(window, width, height);
                }
            }
        );

        glfwSetCursorPosCallback(_windowManager->getWindow(),
            [](GLFWwindow* window, double xpos, double ypos) {
                if (auto control = static_cast<Control*>(glfwGetWindowUserPointer(window))) {
                    control->cursorPosCallback(window, xpos, ypos);
                }
            }
        );

        glfwSetKeyCallback(_windowManager->getWindow(),
            [](GLFWwindow* window, int key, int scancode, int action, int mods) {
                if (auto control = static_cast<Control*>(glfwGetWindowUserPointer(window))) {
                    control->keyCallback(window, key, scancode, action, mods);
                }
            }
        );
    }

    void charCallback(GLFWwindow* window, unsigned int codepoint) override {
        auto str = encodeUTF8(codepoint);
        addUserInput(str);
    }

    void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) override {
        _mouseCurrentPosX = xpos;
        _mouseCurrentPosY = ypos;

        float xPos = static_cast<float>(xpos);
        float yPos = static_cast<float>(ypos);
        _camera->handleMouse(xPos, yPos);
    }

    void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) override {}

    void framebufferSizeCallback(GLFWwindow* window, int width, int height) override {
        _windowManager->getWidth() = width;
        _windowManager->getHeight() = height;
        glViewport(0, 0, width, height);
    }

    void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) override {
        if ((button == GLFW_MOUSE_BUTTON_LEFT) && (action == GLFW_PRESS)) {
        }
    }

    void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) override {
        if (ICommand* command = _inputHandler.handleInput(key, scancode, action, mods)) {
            command->execute();
        }

        if ((key == GLFW_KEY_ENTER) && (action == GLFW_PRESS)) {
            _userInputVector.clear();
        }
        else if ((key == GLFW_KEY_BACKSPACE) && (action == GLFW_PRESS)) {
            if (!_userInputVector.empty()) {
                _userInputVector.pop_back();
            }
        }
        else if ((key == GLFW_KEY_ESCAPE) && (action == GLFW_PRESS)) {
            _windowManager->windowShouldClose();
        }

        if (action == GLFW_RELEASE) {
            //LOGGER(info, "user input: " << getUserInput());
        }
    }

    void processInput(float deltaTime) {
        GLFWwindow* window = _windowManager->getWindow();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            _snake->handleMovement(SnakeMovement::forward);
        }
        else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            _snake->handleMovement(SnakeMovement::backward);
        }
        else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            _snake->handleMovement(SnakeMovement::left);
        }
        else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            _snake->handleMovement(SnakeMovement::right);
        }
    }

    void setInputHandler(InputHandler& inputHandler) {
        _inputHandler = inputHandler;
    }

private:
    Camera* _camera;
    Snake* _snake;
    InputHandler _inputHandler;

};
