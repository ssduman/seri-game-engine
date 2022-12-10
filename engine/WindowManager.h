#pragma once
#pragma warning(disable: 4100)

#include "Logger.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <utility>

struct WindowProperties {
    const char* windowTitle = nullptr;
    bool isFullscreen = false;
    int windowWidth = 1280;
    int windowHeight = 720;
};

class WindowManager {
public:
    WindowManager(WindowProperties windowProperties) : _windowProperties(windowProperties) {}

    ~WindowManager() {
        glfwDestroyWindow(_window);
        glfwTerminate();

        LOGGER(info, "window manager destroyed and terminated successfully");
    }

    bool init() {
        if (!initglfw()) {
            return false;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        if (!createWindow()) {
            return false;
        }

        glfwSwapInterval(1);
        glfwMakeContextCurrent(_window);

        if (!initglew()) {
            return false;
        }

        glEnable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        setErrorCallback();
        setWindowCloseCallback();
        enableDebugOutput();

        LOGGER(info, "window manager created successfully");
        return true;
    }

    int getWidth() {
        return _windowProperties.windowWidth;
    }

    int getHeight() {
        return _windowProperties.windowHeight;
    }

    float getWidthF() {
        return static_cast<float>(_windowProperties.windowWidth);
    }

    float getHeightF() {
        return static_cast<float>(_windowProperties.windowHeight);
    }

    float getAspect() {
        return getWidthF() / getHeightF();
    }

    double getMouseX() {
        getCursorPosition();
        return _mouseXPosition;
    }

    double getMouseY() {
        getCursorPosition();
        return _mouseYPosition;
    }

    std::pair<double, double> getCursorPositions() {
        getCursorPosition();
        return std::make_pair(_mouseXPosition, _mouseYPosition);
    }

    double getTime() {
        return glfwGetTime();
    }

    GLFWwindow* getWindow() {
        return _window;
    }

    float updateDeltaTime() {
        auto currentFrame = getTime();
        _deltaTime = currentFrame - _lastFrame;
        _lastFrame = currentFrame;

        return static_cast<float>(_deltaTime);
    }

    void setCursorPos(double xpos, double ypos) {
        glfwSetCursorPos(_window, xpos, ypos);
    }

    void setCursorPosMiddle() {
        auto xmid = _windowProperties.windowWidth / 2.0;
        auto ymid = _windowProperties.windowHeight / 2.0;
        setCursorPos(xmid, ymid);
    }

    void hideCursor() {
        glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }

    void enableCursor() {
        glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    void disableCursor() {
        glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    void viewport(int x, int y, int width, int height) {
        _windowProperties.windowWidth = width;
        _windowProperties.windowHeight = height;
        glViewport(x, y, _windowProperties.windowWidth, _windowProperties.windowHeight);
    }

    int windowShouldClose() {
        return glfwWindowShouldClose(_window);
    }

    void clear(unsigned int mask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) {
        glClear(mask);
    }

    void clearColor(float red = 0.2f, float green = 0.2f, float blue = 0.2f, float alpha = 1.0f) {
        glClearColor(red, green, blue, alpha);
    }

    void pollEvents() {
        glfwPollEvents();
    }

    void swapBuffers() {
        glfwSwapBuffers(_window);
    }

    void setWindowShouldCloseToTrue() {
        glfwSetWindowShouldClose(_window, GLFW_TRUE);
    }

    void setPointSize(float size) {
        glPointSize(size);
    }

    void setLineWidth(float width) {
        glLineWidth(width);
    }

    void setWindowUserPointer(void* pointer) {
        glfwSetWindowUserPointer(_window, pointer);
    }

    void* getWindowUserPointer() {
        return glfwGetWindowUserPointer(_window);
    }

private:
    void checkGLError() {
        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            LOGGER(error, "gl error occurred: " << err);
        }
    }

    void enableDebugOutput() {
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(
            [](GLenum source,
            GLenum type,
            GLuint id,
            GLenum severity,
            GLsizei length,
            const GLchar* message,
            const void* userParam) {
                std::string sourceStr;
                if (source == GL_DEBUG_SOURCE_API) {
                    sourceStr = "Calls to the OpenGL API";
                }
                else if (source == GL_DEBUG_SOURCE_WINDOW_SYSTEM) {
                    sourceStr = "Calls to a window-system API";
                }
                else if (source == GL_DEBUG_SOURCE_SHADER_COMPILER) {
                    sourceStr = "A compiler for a shading language";
                }
                else if (source == GL_DEBUG_SOURCE_THIRD_PARTY) {
                    sourceStr = "An application associated with OpenGL";
                }
                else if (source == GL_DEBUG_SOURCE_APPLICATION) {
                    sourceStr = "Generated by the user of this application";
                }
                else if (source == GL_DEBUG_SOURCE_OTHER) {
                    sourceStr = "Some source that isn't one of these";
                }
                else {
                    sourceStr = "Unknown source";
                }

                std::string typeStr;
                if (type == GL_DEBUG_TYPE_ERROR) {
                    typeStr = "An error, typically from the API";
                }
                else if (type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR) {
                    typeStr = "Some behavior marked deprecated has been used";
                }
                else if (type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR) {
                    typeStr = "Something has invoked undefined behavior";
                }
                else if (type == GL_DEBUG_TYPE_PORTABILITY) {
                    typeStr = "Some functionality the user relies upon is not portable";
                }
                else if (type == GL_DEBUG_TYPE_PERFORMANCE) {
                    typeStr = "Code has triggered possible performance issues";
                }
                else if (type == GL_DEBUG_TYPE_MARKER) {
                    typeStr = "Command stream annotation";
                }
                else if (type == GL_DEBUG_TYPE_PUSH_GROUP) {
                    typeStr = "Group pushing";
                }
                else if (type == GL_DEBUG_TYPE_POP_GROUP) {
                    typeStr = "Group popping";
                }
                else if (type == GL_DEBUG_TYPE_OTHER) {
                    typeStr = "Some type that isn't one of these";
                }
                else {
                    typeStr = "Unknown type";
                }

                std::string severityStr;
                if (severity == GL_DEBUG_SEVERITY_HIGH) {
                    severityStr = "All OpenGL Errors, shader compilation / linking errors, or highly - dangerous undefined behavior";
                }
                else if (severity == GL_DEBUG_SEVERITY_MEDIUM) {
                    severityStr = "Major performance warnings, shader compilation / linking warnings, or the use of deprecated functionality";
                }
                else if (severity == GL_DEBUG_SEVERITY_LOW) {
                    severityStr = "Redundant state change performance warning, or unimportant undefined behavior";
                }
                else if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) {
                    severityStr = "Anything that isn't an error or performance issue";
                }
                else {
                    severityStr = "Unknown severity";
                }

                LOGGER(debug, "gl debug message:" << "\n"
                    << "severity: " << severityStr << "\n"
                    << "type: " << typeStr << "\n"
                    << "source: " << sourceStr << "\n"
                    << "id: " << id << "\n"
                    << "message: " << message
                );
            }, nullptr);
    }

    void disableDebugOutput() {
        glDisable(GL_DEBUG_OUTPUT);
    }

    void setErrorCallback() {
        glfwSetErrorCallback(
            [](int error, const char* description) {
                LOGGER(error, "glfw error " << error << ": " << description);
            }
        );
    }

    void setWindowCloseCallback() {
        glfwSetWindowCloseCallback(_window,
            [](GLFWwindow* window) {
                LOGGER(info, "window is attempting to close");
            }
        );
    }

    bool initglfw() {
        if (!glfwInit()) {
            LOGGER(error, "glfwInit error");
            return false;
        }

        LOGGER(info, "gflw version '" << glfwGetVersionString() << "' init succeeded");
        return true;
    }

    bool initglew() {
        if (glewInit() != GLEW_OK) {
            LOGGER(error, "glewInit error");
            return false;
        }

        LOGGER(info, "glew version '" << glewGetString(GLEW_VERSION) << "' init succeeded");
        return true;
    }

    bool createWindow() {
        if (_windowProperties.isFullscreen) {
            GLFWmonitor* glfwMonitor = glfwGetPrimaryMonitor();
            if (!glfwMonitor) {
                LOGGER(error, "getting glfw monitor failed");
                return false;
            }

            int monXPos, monYPos;
            glfwGetMonitorWorkarea(glfwMonitor, &monXPos, &monYPos, &_windowProperties.windowWidth, &_windowProperties.windowHeight);
            _window = glfwCreateWindow(_windowProperties.windowWidth, _windowProperties.windowHeight, _windowProperties.windowTitle, glfwMonitor, nullptr);
        }
        else {
            _window = glfwCreateWindow(_windowProperties.windowWidth, _windowProperties.windowHeight, _windowProperties.windowTitle, nullptr, nullptr);
        }

        if (!_window) {
            LOGGER(error, "glfw window creating error");
            return false;
        }

        LOGGER(info, "glfw window created");
        return true;
    }

    void getCursorPosition() {
        glfwGetCursorPos(_window, &_mouseXPosition, &_mouseYPosition);
    }

    WindowProperties _windowProperties;
    GLFWwindow* _window{ nullptr };

    double _lastFrame{ 0.0 };
    double _deltaTime{ 0.0 };
    double _mouseXPosition{ 0.0 };
    double _mouseYPosition{ 0.0 };

};

