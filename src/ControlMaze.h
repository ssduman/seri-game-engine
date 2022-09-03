#pragma once
#pragma warning(disable: 4100)
#pragma warning(disable: 4244)

#include "WindowManager.h"
#include "IControl.h"
#include "InputHandler.h"
#include "CameraMaze.h"
#include "Maze.h"

class ControlMaze : public IControl {
public:
    ControlMaze(WindowManager* windowManager, CameraMaze** camera, Maze* maze) : IControl(windowManager), _camera(camera), _maze(maze) {
        glfwSetWindowUserPointer(_windowManager->getWindow(), static_cast<void*>(this));

        init();

        LOGGER(info, "control init succeeded");
    }

    virtual ~ControlMaze() {}

    void charCallback(GLFWwindow* window, unsigned int codepoint) override {
        char string[5] = "";
        encode_utf8(string, codepoint);
        _userInputVector.push_back(std::string(string));
    }

    void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) override {
        _mouseCurrentPosX = xpos;
        _mouseCurrentPosY = ypos;

        float xPos = static_cast<float>(xpos);
        float yPos = static_cast<float>(ypos);
        (*_camera)->handleMouse(xPos, yPos);
    }

    void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) override {
        float& ambient = (*_camera)->getAmbient();
        if ((yoffset > 0) && (ambient <= 0.98f)) {
            ambient += 0.02f;
        } else if ((yoffset < 0) && (ambient >= 0.02f)) {
            ambient -= 0.02f;
        }
    }

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
        if (key == GLFW_KEY_ESCAPE) {
            _windowManager->windowShouldClose();
            return;
        }

        if ((key == GLFW_KEY_ENTER) && (action == GLFW_PRESS)) {
            auto userInput = getUserInput();

            if (userInput.size() < 3) {
                return;
            }

            std::string part;
            std::vector<std::string> parts;
            std::stringstream userInputStream(userInput);
            while (std::getline(userInputStream, part, 'x')) {
                parts.push_back(part);
            }
            if (userInput == part || parts[0].size() == 0 || parts.size() != 2) {
                return;
            }
            std::stringstream width_s(parts[0]);
            std::stringstream height_s(parts[1]);
            _userInputVector.clear();

            float mazeWidth, mazeHeight, thickness = 5.0f;
            width_s >> mazeWidth;
            height_s >> mazeHeight;
            if (mazeWidth == 0 || mazeHeight == 0) {
                return;
            }

            delete _maze, * _camera;
            _maze = new Maze(thickness, mazeWidth, mazeHeight);

            CameraProperties cameraProperties;
            cameraProperties.position = glm::vec3((mazeWidth - 1) * thickness * 2, -thickness / 2, -thickness * 4);
            (*_camera) = new CameraMaze(cameraProperties);
            (*_camera)->getIsPlaying() = true;
            (*_camera)->setDimensions(mazeWidth, mazeHeight, thickness);
            (*_camera)->setWallPos(_maze->getVerticalWallPosition(), _maze->getHorizontalWallPosition());
        } else if ((key == GLFW_KEY_BACKSPACE) && (action == GLFW_PRESS)) {
            if (_userInputVector.size() > 0) {
                _userInputVector.pop_back();
            }
        }

        if (action == GLFW_RELEASE) {
            //LOGGER(info, "user input: " << getUserInput());
        }
    }

private:
    void init() override {
        glfwSetCharCallback(_windowManager->getWindow(),
            [](GLFWwindow* window, unsigned int codepoint) {
                if (auto control = static_cast<ControlMaze*>(glfwGetWindowUserPointer(window))) {
                    control->charCallback(window, codepoint);
                }
            }
        );

        glfwSetMouseButtonCallback(_windowManager->getWindow(),
            [](GLFWwindow* window, int button, int action, int mods) {
                if (auto control = static_cast<ControlMaze*>(glfwGetWindowUserPointer(window))) {
                    control->mouseButtonCallback(window, button, action, mods);
                }
            }
        );

        glfwSetScrollCallback(_windowManager->getWindow(),
            [](GLFWwindow* window, double xoffset, double yoffset) {
                if (auto control = static_cast<ControlMaze*>(glfwGetWindowUserPointer(window))) {
                    control->scrollCallback(window, xoffset, yoffset);
                }
            }
        );

        glfwSetFramebufferSizeCallback(_windowManager->getWindow(),
            [](GLFWwindow* window, int width, int height) {
                if (auto control = static_cast<ControlMaze*>(glfwGetWindowUserPointer(window))) {
                    control->framebufferSizeCallback(window, width, height);
                }
            }
        );

        glfwSetCursorPosCallback(_windowManager->getWindow(),
            [](GLFWwindow* window, double xpos, double ypos) {
                if (auto control = static_cast<ControlMaze*>(glfwGetWindowUserPointer(window))) {
                    control->cursorPosCallback(window, xpos, ypos);
                }
            }
        );

        glfwSetKeyCallback(_windowManager->getWindow(),
            [](GLFWwindow* window, int key, int scancode, int action, int mods) {
                if (auto control = static_cast<ControlMaze*>(glfwGetWindowUserPointer(window))) {
                    control->keyCallback(window, key, scancode, action, mods);
                }
            }
        );
    }

    CameraMaze** _camera;
    Maze* _maze;
};
