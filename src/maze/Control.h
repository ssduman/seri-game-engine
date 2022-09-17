#pragma once
#pragma warning(disable: 4100)
#pragma warning(disable: 4244)

#include "../engine/IControl.h"

#include "Maze.h"
#include "Camera.h"

class Control : public IControl {
public:
    Control(WindowManager* windowManager, Camera* camera, Maze** maze) : IControl(windowManager), _camera(camera), _maze(maze) {
        glfwSetWindowUserPointer(_windowManager->getWindow(), static_cast<void*>(this));

        Control::init();

        LOGGER(info, "control init succeeded");
    }

    ~Control() override = default;

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

    void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) override {
        float ambient = _camera->getAmbient();
        if ((yoffset > 0) && (ambient <= 0.98f)) {
            _camera->incrementAmbient(+0.02f);
        } else if ((yoffset < 0) && (ambient >= 0.02f)) {
            _camera->incrementAmbient(-0.02f);
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
            if (getUserInputSize() < 3) {
                return;
            }

            auto userInput = getUserInput();

            std::string part;
            std::vector<std::string> parts;
            std::stringstream userInputStream(userInput);
            while (std::getline(userInputStream, part, 'x')) {
                parts.push_back(part);
            }
            if (userInput == part || parts[0].empty() || parts.size() != 2) {
                return;
            }
            std::stringstream width_s(parts[0]);
            std::stringstream height_s(parts[1]);
            clearUserInput();

            float mazeWidth, mazeHeight, mazeThickness = 5.0f;
            width_s >> mazeWidth;
            height_s >> mazeHeight;
            if (mazeWidth < 1.0f || mazeHeight < 1.0f) {
                return;
            }

            generateMaze(mazeWidth, mazeHeight, mazeThickness);
            return;
        }

        if ((key == GLFW_KEY_R) && (action == GLFW_PRESS)) {
            generateMaze((*_maze)->getMazeWidth(), (*_maze)->getMazeHeight(), (*_maze)->getMazeThickness());
            return;
        }

        if ((key == GLFW_KEY_BACKSPACE) && (action == GLFW_PRESS)) {
            deleteLastUserInput();
        }

        if (action == GLFW_RELEASE) {
            //auto i = getUserInput();
            //auto s = getUserInputSize();
            //LOGGER(info, "user input: " << i << ", size: " << s);
        }
    }

    void generateMaze(float mazeWidth, float mazeHeight, float mazeThickness) {
        delete* _maze;
        *_maze = new Maze{ _camera, mazeWidth, mazeHeight, mazeThickness };

        CameraProperties cameraProperties;
        cameraProperties.position = glm::vec3((mazeWidth - 1) * mazeThickness * 2, mazeThickness / 2, -mazeThickness * 4);
        _camera->getCameraProperties() = cameraProperties;
        _camera->setCameraPosition(cameraProperties.position);
        _camera->getIsPlaying() = true;
        _camera->showEscapePath() = false;
        _camera->isRestartTriggered() = true;
        _camera->setMazeDimensions(mazeWidth, mazeHeight, mazeThickness);
        _camera->setMazeWallPositions((*_maze)->getVerticalWallPosition(), (*_maze)->getHorizontalWallPosition());
        _camera->init();
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
    Maze** _maze;
};
