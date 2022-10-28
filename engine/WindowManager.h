#pragma once
#pragma warning(disable: 4100)

#include "Logger.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

struct WindowProperties {
    const char* windowTitle = nullptr;
    bool isFullscreen = false;
    int windowWidth = 1280;
    int windowHeight = 720;
};

class WindowManager {
public:
    WindowManager(WindowProperties& windowProperties) : _windowProperties(windowProperties) {
        if (!glfwInit()) {
            LOGGER(error, "glfwInit error");
            exit(-1);
        }
        LOGGER(info, "gflw version '" << glfwGetVersionString() << "' init succeeded");

        glfwSetErrorCallback(
            [](int error, const char* description) {
                LOGGER(error, "glfw error " << error << ": " << description);
            }
        );

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        //glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
        //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

        if (_windowProperties.isFullscreen) {
            int monXPos, monYPos;
            glfwGetMonitorWorkarea(glfwGetPrimaryMonitor(), &monXPos, &monYPos, &_windowProperties.windowWidth, &_windowProperties.windowHeight);
            _window = glfwCreateWindow(_windowProperties.windowWidth, _windowProperties.windowHeight, _windowProperties.windowTitle, glfwGetPrimaryMonitor(), nullptr);
        }
        else {
            _window = glfwCreateWindow(_windowProperties.windowWidth, _windowProperties.windowHeight, _windowProperties.windowTitle, nullptr, nullptr);
        }
        if (!_window) {
            glfwTerminate();
            LOGGER(error, "window creating error");
            exit(-1);
        }

        glfwMakeContextCurrent(_window);
        glfwSwapInterval(1);

        if (glewInit() != GLEW_OK) {
            LOGGER(error, "glewInit error");
            exit(-1);
        }
        LOGGER(info, "glew version '" << glewGetString(GLEW_VERSION) << "' init succeeded");

        glfwSetWindowCloseCallback(_window,
            [](GLFWwindow* window) {
                LOGGER(info, "window is attempting to close");
            }
        );

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(
            [](GLenum source,
            GLenum type,
            GLuint id,
            GLenum severity,
            GLsizei length,
            const GLchar* message,
            const void* userParam) {
                LOGGER(debug, "gl message -> source: " << source << ", type: " << type << ", id: " << id << ", severity: " << severity << ", message: " << message);
        }, nullptr);

        setPointSize(10.0f);
        //setLineWidth(10.0f);

        LOGGER(info, "window created successfully");
    }

    ~WindowManager() {
        glfwDestroyWindow(_window);
        glfwTerminate();

        LOGGER(info, "window destroyed and terminated successfully");
    }

    int& getWidth() {
        return _windowProperties.windowWidth;
    }

    int& getHeight() {
        return _windowProperties.windowHeight;
    }

    float getWidthF() {
        return static_cast<float>(_windowProperties.windowWidth);
    }

    float getHeightF() {
        return static_cast<float>(_windowProperties.windowHeight);
    }

    double getMouseX() {
        getMousePosition();
        return _mouseXPosition;
    }

    double getMouseY() {
        getMousePosition();
        return _mouseYPosition;
    }

    void setCursorPos(double xpos, double ypos) {
        glfwSetCursorPos(_window, xpos, ypos);
    }

    void setCursorPosMiddle() {
        auto xmid = _windowProperties.windowWidth / 2.0;
        auto ymid = _windowProperties.windowHeight / 2.0;
        setCursorPos(xmid, ymid);
    }

    GLFWwindow* getWindow() {
        return _window;
    }

    double getTime() {
        return glfwGetTime();
    }

    float updateDeltaTime() {
        auto currentFrame = getTime();
        _deltaTime = currentFrame - _lastFrame;
        _lastFrame = currentFrame;

        return static_cast<float>(_deltaTime);
    }

    void enableCursor() {
        glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    void hideCursor() {
        glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }

    void disableCursor() {
        glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    void windowShouldClose() {
        glfwSetWindowShouldClose(_window, GLFW_TRUE);
    }

    void setPointSize(float size) {
        glPointSize(size);
    }

    void setLineWidth(float width) {
        glLineWidth(width);
    }

    void checkGLError() {
        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            LOGGER(error, "gl error occurred: " << err);
        }
    }

private:
    void getMousePosition() {
        glfwGetCursorPos(_window, &_mouseXPosition, &_mouseYPosition);
    }

    double _lastFrame = 0.0;
    double _deltaTime = 0.0;
    double _mouseXPosition = 0.0;
    double _mouseYPosition = 0.0;
    GLFWwindow* _window = nullptr;
    WindowProperties& _windowProperties;

};

