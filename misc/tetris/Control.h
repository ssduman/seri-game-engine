#pragma once
#pragma warning(disable: 4100)
#pragma warning(disable: 4244)

#include "../engine/IControl.h"
#include "../engine/InputHandler.h"
#include "../engine/WindowManager.h"

#include "Camera.h"

class Control : public IControl {
public:
    Control(WindowManager* windowManager, Camera* camera, State* state, Tetris& tetris)
        : IControl(windowManager, state), _camera(camera), _window(_windowManager->getWindow()), _tetris(tetris) {
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
        if (isPressed(key, action, GLFW_KEY_ESCAPE)) {
            glfwSetWindowShouldClose(_window, GLFW_TRUE);
        }
        if (isPressed(key, action, GLFW_KEY_S) || isPressed(key, action, GLFW_KEY_DOWN)) {
            _tetris.setRequestedBlockMovement(BlockMovement::faster_down);
        }
        if (isPressed(key, action, GLFW_KEY_A) || isPressed(key, action, GLFW_KEY_LEFT)) {
            _tetris.setRequestedBlockMovement(BlockMovement::left);
        }
        if (isPressed(key, action, GLFW_KEY_D) || isPressed(key, action, GLFW_KEY_RIGHT)) {
            _tetris.setRequestedBlockMovement(BlockMovement::right);
        }
        if (isPressed(key, action, GLFW_KEY_Q)) {
            _tetris.setRequestedBlockMovement(BlockMovement::rotate_left);
        }
        if (isPressed(key, action, GLFW_KEY_E)) {
            _tetris.setRequestedBlockMovement(BlockMovement::rotate_right);
        }
        if (isPressed(key, action, GLFW_KEY_P)) {
            LOGGER(info, "requested puase");
        }
    }

private:
    bool isPressed(int key, int action, int target) {
        if ((key == target) && (action == GLFW_PRESS)) {
            return true;
        }
        return false;
    }

    Camera* _camera;
    GLFWwindow* _window;
    Tetris& _tetris;

};
