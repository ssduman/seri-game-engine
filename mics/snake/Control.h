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
        : IControl(windowManager, state), _camera(camera), _snake(snake), _window(_windowManager->getWindow()) {
        glfwSetWindowUserPointer(_window, static_cast<void*>(this));

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

    void charCallback(GLFWwindow* window, unsigned int codepoint) override {}

    void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) override {
        _mouseCurrentPosX = xpos;
        _mouseCurrentPosY = ypos;
    }

    void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) override {}

    void framebufferSizeCallback(GLFWwindow* window, int width, int height) override {
        _windowManager->getWidth() = width;
        _windowManager->getHeight() = height;
        glViewport(0, 0, width, height);
    }

    void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) override {}

    void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) override {
        if ((key == GLFW_KEY_P) && (action == GLFW_PRESS)) {
            _snake->toggleIsPlaying();
        }
    }

    void processInput(float deltaTime) {
        if (glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(_window, GLFW_TRUE);
        }

        if (glfwGetKey(_window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(_window, GLFW_KEY_UP) == GLFW_PRESS) {
            _snake->handleMovement(deltaTime, SnakeMovement::forward);
        }
        else if (glfwGetKey(_window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(_window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            _snake->handleMovement(deltaTime, SnakeMovement::backward);
        }
        else if (glfwGetKey(_window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(_window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            _snake->handleMovement(deltaTime, SnakeMovement::left);
        }
        else if (glfwGetKey(_window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(_window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            _snake->handleMovement(deltaTime, SnakeMovement::right);
        }
    }

private:
    Camera* _camera;
    Snake* _snake;
    GLFWwindow* _window;

};
