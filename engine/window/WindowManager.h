#pragma once
#pragma warning(disable: 4100)

#include "../logging/Logger.h"

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
        constexpr auto getDebugSourceString = [](GLenum source) {
            if (source == GL_DEBUG_SOURCE_API) {
                return "Calls to the OpenGL API";
            }
            if (source == GL_DEBUG_SOURCE_WINDOW_SYSTEM) {
                return "Calls to a window-system API";
            }
            if (source == GL_DEBUG_SOURCE_SHADER_COMPILER) {
                return "A compiler for a shading language";
            }
            if (source == GL_DEBUG_SOURCE_THIRD_PARTY) {
                return "An application associated with OpenGL";
            }
            if (source == GL_DEBUG_SOURCE_APPLICATION) {
                return "Generated by the user of this application";
            }
            if (source == GL_DEBUG_SOURCE_OTHER) {
                return "Some source that isn't one of these";
            }

            return "Unknown source";
        };

        constexpr auto getDebugTypeString = [](GLenum type) {
            if (type == GL_DEBUG_TYPE_ERROR) {
                return "An error, typically from the API";
            }
            if (type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR) {
                return "Some behavior marked deprecated has been used";
            }
            if (type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR) {
                return "Something has invoked undefined behavior";
            }
            if (type == GL_DEBUG_TYPE_PORTABILITY) {
                return "Some functionality the user relies upon is not portable";
            }
            if (type == GL_DEBUG_TYPE_PERFORMANCE) {
                return "Code has triggered possible performance issues";
            }
            if (type == GL_DEBUG_TYPE_MARKER) {
                return "Command stream annotation";
            }
            if (type == GL_DEBUG_TYPE_PUSH_GROUP) {
                return "Group pushing";
            }
            if (type == GL_DEBUG_TYPE_POP_GROUP) {
                return "Group popping";
            }
            if (type == GL_DEBUG_TYPE_OTHER) {
                return "Some type that isn't one of these";
            }

            return "Unknown type";
        };

        constexpr auto getDebugSeverityString = [](GLenum severity) {
            if (severity == GL_DEBUG_SEVERITY_HIGH) {
                return "All OpenGL Errors, shader compilation / linking errors, or highly - dangerous undefined behavior";
            }
            else if (severity == GL_DEBUG_SEVERITY_MEDIUM) {
                return "Major performance warnings, shader compilation / linking warnings, or the use of deprecated functionality";
            }
            else if (severity == GL_DEBUG_SEVERITY_LOW) {
                return "Redundant state change performance warning, or unimportant undefined behavior";
            }
            else if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) {
                return "Anything that isn't an error or performance issue";
            }

            return "Unknown source";
        };

        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei len, const GLchar* message, const void* param) {
            LOGGER(debug, "gl debug message:" << "\n"
            << "severity: " << getDebugSeverityString(severity) << "\n"
                               << "type: " << getDebugTypeString(type) << "\n"
                               << "source: " << getDebugSourceString(source) << "\n"
                               << "id: " << id << "\n"
                               << "message: " << message);
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

