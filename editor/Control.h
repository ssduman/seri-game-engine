#pragma once
#pragma warning(disable: 4100)
#pragma warning(disable: 4244)

#include "../engine/core/Seri.h"

#include "Camera.h"

#include <memory>

class Control : public IControl {
public:
    Control(std::shared_ptr<WindowManager> windowManager, std::shared_ptr<State> state, std::shared_ptr<Camera> camera)
        : IControl(windowManager, state), _camera(camera) {
        _windowManager->setWindowUserPointer(static_cast<void*>(this));

        LOGGER(info, "control init succeeded");
    }

    ~Control() override {
        LOGGER(info, "control delete succeeded");
    }

    void charCallback(GLFWwindow* window, unsigned int codepoint) override {
        addUserInput(encodeUTF8(codepoint));
    }

    void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) override {
        _camera->handleMouse(static_cast<float>(xpos), static_cast<float>(ypos));
    }

    void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) override {}

    void framebufferSizeCallback(GLFWwindow* window, int width, int height) override {
        _windowManager->viewport(0, 0, width, height);
        _camera->updateAspect(_windowManager->getAspect());
    }

    void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) override {
        if ((button == GLFW_MOUSE_BUTTON_LEFT) && (action == GLFW_PRESS)) {
        }
    }

    void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) override {
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
            _windowManager->setWindowShouldCloseToTrue();
        }

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            _camera->handleInput(deltaTime, CameraMovement::forward);
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            _camera->handleInput(deltaTime, CameraMovement::backward);
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            _camera->handleInput(deltaTime, CameraMovement::left);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            _camera->handleInput(deltaTime, CameraMovement::right);
        }
    }

private:
    std::shared_ptr<Camera> _camera;

};
