#pragma once
#pragma warning(disable: 4100)

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

struct WindowProperties {
    const char* windowTitle = nullptr;
    bool isFullscreen = false;
    int windowWidth = 1280;
    int windowHeight = 720;
};

class Window {
public:
    Window(WindowProperties windowProperties) : _windowProperties(windowProperties) {
        if (!glfwInit()) {
            std::cerr << "glfwInit error" << std::endl;
            exit(-1);
        }
        std::cout << "gflw version '" << glfwGetVersionString() << "' init succeeded" << std::endl;

        glfwSetErrorCallback(
            [](int error, const char* description) {
                std::cerr << "glfw error " << error << ": " << description << std::endl;
            }
        );

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

        if (_windowProperties.isFullscreen) {
            int monXPos, monYPos;
            glfwGetMonitorWorkarea(glfwGetPrimaryMonitor(), &monXPos, &monYPos, &_windowProperties.windowWidth, &_windowProperties.windowHeight);
            _window = glfwCreateWindow(_windowProperties.windowWidth, _windowProperties.windowHeight, _windowProperties.windowTitle, glfwGetPrimaryMonitor(), NULL);
        } else {
            _window = glfwCreateWindow(_windowProperties.windowWidth, _windowProperties.windowHeight, _windowProperties.windowTitle, NULL, NULL);
        }
        if (!_window) {
            glfwTerminate();
            std::cerr << "window creating error" << std::endl;
            exit(-1);
        }

        glfwMakeContextCurrent(_window);
        glfwSwapInterval(1);

        if (glewInit() != GLEW_OK) {
            std::cerr << "glewInit error" << std::endl;
            exit(-1);
        }
        std::cout << "glew version '" << glewGetString(GLEW_VERSION) << "' init succeeded" << std::endl;

        glfwSetWindowCloseCallback(_window,
            [](GLFWwindow* window) {
                std::cerr << "window is attempting to close" << std::endl;
            }
        );

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    inline int getWidth() {
        return _windowProperties.windowWidth;
    }

    inline int getHeight() {
        return _windowProperties.windowHeight;
    }

    inline double getMouseX() {
        getMousePosition();
        return _mouseXPosition;
    }

    inline double getMouseY() {
        getMousePosition();
        return _mouseYPosition;
    }

    inline void setCursorPosMiddle() {
        glfwSetCursorPos(_window, (double)_windowProperties.windowWidth / 2.0, (double)_windowProperties.windowHeight / 2.0);
    }

    inline GLFWwindow* getWindow() {
        return _window;
    }

    inline double getTime() {
        return glfwGetTime();
    }

private:
    inline void getMousePosition() {
        glfwGetCursorPos(_window, &_mouseXPosition, &_mouseYPosition);
    }

    double _mouseXPosition = 0;
    double _mouseYPosition = 0;
    GLFWwindow* _window = nullptr;
    WindowProperties _windowProperties;

};

