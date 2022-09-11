#pragma once
#pragma warning(disable: 4100)
#pragma warning(disable: 4244)

#include "WindowManager.h"
#include "IControl.h"
#include "InputHandler.h"

class Control : public IControl {
public:
    Control(WindowManager* windowManager, Camera* camera, State* state) : IControl(windowManager, state), _camera(camera), _inputHandler(camera) {
        glfwSetWindowUserPointer(_windowManager->getWindow(), static_cast<void*>(this));

        Control::init();

        LOGGER(info, "control init succeeded");
    }

    ~Control() override = default;

    void charCallback(GLFWwindow* window, unsigned int codepoint) override {
        char string[5] = "";
        encode_utf8(string, codepoint);
        _userInputVector.emplace_back(string);
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
        } else if ((key == GLFW_KEY_BACKSPACE) && (action == GLFW_PRESS)) {
            if (!_userInputVector.empty()) {
                _userInputVector.pop_back();
            }
        } else if ((key == GLFW_KEY_ESCAPE) && (action == GLFW_PRESS)) {
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
            _camera->handleInput(deltaTime, CameraMovement::FORWARD);
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            _camera->handleInput(deltaTime, CameraMovement::BACKWARD);
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            _camera->handleInput(deltaTime, CameraMovement::LEFT);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            _camera->handleInput(deltaTime, CameraMovement::RIGHT);
        }
    }

    void setInputHandler(InputHandler& inputHandler) {
        _inputHandler = inputHandler;
    }

private:
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

    Camera* _camera;
    InputHandler _inputHandler;
};
