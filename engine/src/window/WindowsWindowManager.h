#pragma once
#pragma warning(disable: 4100)

#include "IWindowManager.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <utility>
#include <stdexcept>

class WindowsWindowManager : public IWindowManager {
public:
    WindowsWindowManager() = default;

    ~WindowsWindowManager() override {
        glfwDestroyWindow(_window);
        glfwTerminate();

        LOGGER(info, "windows window manager destroyed and terminated successfully");
    }

    void init() override {
        if (_initialized) {
            throw std::runtime_error("window manager is already initialized");
        }

        initglfw();

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        createWindow();
        glfwSwapInterval(1);
        glfwMakeContextCurrent(_window);

        initglad();
        logInfoStrings();

        glEnable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        setWindowUserPointer(static_cast<void*>(this));
        setEventCallbacks();

        _initialized = true;

        LOGGER(info, "window manager created successfully");
    }

    double getTime() override {
        return glfwGetTime();
    }

    void updateDeltaTime() override {
        auto currentFrame = getTime();
        _deltaTime = currentFrame - _lastFrame;
        _lastFrame = currentFrame;
    }

    float getDeltaTime() override {
        return static_cast<float>(_deltaTime);
    }

    void* getWindow() override {
        return _window;
    }

    void hideCursor() override {
        glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }

    void enableCursor() override {
        glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    void disableCursor() override {
        glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    double getCursorX() override {
        auto [x, y] = getCursorPosition();
        return x;
    }

    double getCursorY() override {
        auto [x, y] = getCursorPosition();
        return y;
    }

    std::pair<double, double> getCursorPosition() override {
        double mouseXPosition, mouseYPosition;
        glfwGetCursorPos(_window, &mouseXPosition, &mouseYPosition);
        return { mouseXPosition, mouseYPosition };
    }

    void setCursorPosition(double xpos, double ypos) override {
        glfwSetCursorPos(_window, xpos, ypos);
    }

    void viewport(int x, int y, int width, int height) override {
        _windowProperties.windowWidth = width;
        _windowProperties.windowHeight = height;
        glViewport(x, y, _windowProperties.windowWidth, _windowProperties.windowHeight);
    }

    int windowShouldClose() override {
        return glfwWindowShouldClose(_window);
    }

    void clear() override {
        glClear(getClearMask());
    }

    void clearColor(float red = 0.2f, float green = 0.2f, float blue = 0.2f, float alpha = 1.0f) override {
        glClearColor(red, green, blue, alpha);
    }

    void pollEvents() override {
        glfwPollEvents();
    }

    void swapBuffers() override {
        glfwSwapBuffers(_window);
    }

    void setWindowShouldCloseToTrue() override {
        glfwSetWindowShouldClose(_window, GLFW_TRUE);
    }

    void setPointSize(float size) override {
        glPointSize(size);
    }

    void setLineWidth(float width) override {
        glLineWidth(width);
    }

    std::pair<int, int> getWindowPosition() override {
        int xpos, ypos;
        glfwGetWindowPos(_window, &xpos, &ypos);
        return { xpos, ypos };
    }

    void setWindowPosition(int xpos, int ypos) override {
        glfwSetWindowPos(_window, xpos, ypos);
    }

    const char* getClipboard() override {
        return glfwGetClipboardString(nullptr);
    }

    void setClipboard(const char* str) override {
        glfwSetClipboardString(nullptr, str);
    }

    void setWindowUserPointer(void* pointer) {
        glfwSetWindowUserPointer(_window, pointer);
    }

    void* getWindowUserPointer() {
        return glfwGetWindowUserPointer(_window);
    }

    void fireEvent(IEventData& data) override {
        _eventCallback->fireEvent(data);
    }

private:
    void initglfw() {
        if (!glfwInit()) {
            throw std::runtime_error("glfw init error");
        }

        LOGGER(info, "gflw version '" << glfwGetVersionString() << "' init succeeded");
    }

    void initglad() {
        int version = gladLoadGL(glfwGetProcAddress);
        if (version == 0) {
            throw std::runtime_error("glad load error");
        }

        LOGGER(info, "loaded opengl " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version));
    }

    void createWindow() {
        if (_windowProperties.isFullscreen) {
            GLFWmonitor* glfwMonitor = glfwGetPrimaryMonitor();
            if (!glfwMonitor) {
                throw std::runtime_error("getting glfw monitor failed");
            }

            int monXPos, monYPos;
            glfwGetMonitorWorkarea(glfwMonitor, &monXPos, &monYPos, &_windowProperties.windowWidth, &_windowProperties.windowHeight);
            _window = glfwCreateWindow(_windowProperties.windowWidth, _windowProperties.windowHeight, _windowProperties.windowTitle, glfwMonitor, nullptr);
        }
        else {
            _window = glfwCreateWindow(_windowProperties.windowWidth, _windowProperties.windowHeight, _windowProperties.windowTitle, nullptr, nullptr);
        }

        if (!_window) {
            throw std::runtime_error("glfw window creating error");
        }

        LOGGER(info, "glfw window created");
    }

    void logInfoStrings() {
        LOGGER(info, "vendor: " << glGetString(GL_VENDOR));
        LOGGER(info, "version: " << glGetString(GL_VERSION));
        LOGGER(info, "renderer: " << glGetString(GL_RENDERER));
        LOGGER(info, "shading language version: " << glGetString(GL_SHADING_LANGUAGE_VERSION));
    }

    void setEventCallbacks() {
        // input
        glfwSetKeyCallback(_window,
            [](GLFWwindow* window, int key, int scancode, int action, int mods) {
                if (auto windowManager = static_cast<WindowsWindowManager*>(glfwGetWindowUserPointer(window))) {
                    auto keyEnum = static_cast<KeyCode>(key);
                    auto actionEnum = static_cast<InputAction>(action);

                    std::vector<InputModifier> modsVector;
                    if (mods & static_cast<int>(InputModifier::alt)) {
                        modsVector.emplace_back(InputModifier::alt);
                    }
                    if (mods & static_cast<int>(InputModifier::shift)) {
                        modsVector.emplace_back(InputModifier::shift);
                    }
                    if (mods & static_cast<int>(InputModifier::super)) {
                        modsVector.emplace_back(InputModifier::super);
                    }
                    if (mods & static_cast<int>(InputModifier::control)) {
                        modsVector.emplace_back(InputModifier::control);
                    }
                    if (mods & static_cast<int>(InputModifier::num_lock)) {
                        modsVector.emplace_back(InputModifier::num_lock);
                    }
                    if (mods & static_cast<int>(InputModifier::caps_lock)) {
                        modsVector.emplace_back(InputModifier::caps_lock);
                    }
                    if (modsVector.empty()) {
                        modsVector.emplace_back(InputModifier::noop);
                    }

                    windowManager->fireEvent(KeyEventData{ keyEnum, scancode, actionEnum, std::move(modsVector) });

                    if (keyEnum == KeyCode::escape && actionEnum == InputAction::press) {
                        windowManager->setWindowShouldCloseToTrue();
                    }
                }
            }
        );

        glfwSetCharCallback(_window,
            [](GLFWwindow* window, unsigned int codepoint) {
                if (auto windowManager = static_cast<WindowsWindowManager*>(glfwGetWindowUserPointer(window))) {
                    windowManager->fireEvent(CharacterEventData{ codepoint });
                }
            }
        );

        glfwSetCharModsCallback(_window,
            [](GLFWwindow* window, unsigned int codepoint, int mods) {
                if (auto windowManager = static_cast<WindowsWindowManager*>(glfwGetWindowUserPointer(window))) {
                    std::vector<InputModifier> modsVector;
                    if (mods & static_cast<int>(InputModifier::alt)) {
                        modsVector.emplace_back(InputModifier::alt);
                    }
                    if (mods & static_cast<int>(InputModifier::shift)) {
                        modsVector.emplace_back(InputModifier::shift);
                    }
                    if (mods & static_cast<int>(InputModifier::super)) {
                        modsVector.emplace_back(InputModifier::super);
                    }
                    if (mods & static_cast<int>(InputModifier::control)) {
                        modsVector.emplace_back(InputModifier::control);
                    }
                    if (mods & static_cast<int>(InputModifier::num_lock)) {
                        modsVector.emplace_back(InputModifier::num_lock);
                    }
                    if (mods & static_cast<int>(InputModifier::caps_lock)) {
                        modsVector.emplace_back(InputModifier::caps_lock);
                    }
                    if (modsVector.empty()) {
                        modsVector.emplace_back(InputModifier::noop);
                    }

                    windowManager->fireEvent(CharacterModsEventData{ codepoint, std::move(modsVector) });
                }
            }
        );

        // mouse
        glfwSetCursorEnterCallback(_window,
            [](GLFWwindow* window, int entered) {
                if (auto windowManager = static_cast<WindowsWindowManager*>(glfwGetWindowUserPointer(window))) {
                    windowManager->fireEvent(MouseEnterEventData{ entered ? true : false });
                }
            }
        );

        glfwSetMouseButtonCallback(_window,
            [](GLFWwindow* window, int button, int action, int mods) {
                if (auto windowManager = static_cast<WindowsWindowManager*>(glfwGetWindowUserPointer(window))) {
                    auto buttonEnum = static_cast<MouseButtonCode>(button);
                    auto actionEnum = static_cast<InputAction>(action);
                    auto modsEnum = static_cast<InputModifier>(mods);

                    windowManager->fireEvent(MouseButtonEventData{ buttonEnum, actionEnum, modsEnum });
                }
            }
        );

        glfwSetScrollCallback(_window,
            [](GLFWwindow* window, double xoffset, double yoffset) {
                if (auto windowManager = static_cast<WindowsWindowManager*>(glfwGetWindowUserPointer(window))) {
                    windowManager->fireEvent(MouseScrollEventData{ xoffset, yoffset });
                }
            }
        );

        glfwSetCursorPosCallback(_window,
            [](GLFWwindow* window, double xpos, double ypos) {
                if (auto windowManager = static_cast<WindowsWindowManager*>(glfwGetWindowUserPointer(window))) {
                    windowManager->fireEvent(MousePositionEventData{ xpos, ypos });
                }
            }
        );

        // window
        glfwSetDropCallback(_window,
            [](GLFWwindow* window, int path_count, const char* paths[]) {
                if (auto windowManager = static_cast<WindowsWindowManager*>(glfwGetWindowUserPointer(window))) {
                    std::vector<std::string> pathVector;
                    for (int i = 0; i < path_count; i++) {
                        pathVector.emplace_back(paths[i]);
                    }

                    windowManager->fireEvent(WindowDropEventData{ std::move(pathVector) });
                }
            }
        );

        glfwSetWindowCloseCallback(_window,
            [](GLFWwindow* window) {
                if (auto windowManager = static_cast<WindowsWindowManager*>(glfwGetWindowUserPointer(window))) {
                    windowManager->fireEvent(WindowCloseEventData{});
                }
            }
        );

        glfwSetFramebufferSizeCallback(_window,
            [](GLFWwindow* window, int width, int height) {
                if (auto windowManager = static_cast<WindowsWindowManager*>(glfwGetWindowUserPointer(window))) {
                    windowManager->fireEvent(WindowResizeEventData{ width, height });

                    windowManager->viewport(0, 0, width, height);
                }
            }
        );

        glfwSetWindowPosCallback(_window,
            [](GLFWwindow* window, int xpos, int ypos) {
                if (auto windowManager = static_cast<WindowsWindowManager*>(glfwGetWindowUserPointer(window))) {
                    LOGGER(verbose, "window new position: " << xpos << ", " << ypos);
                }
            }
        );

        glfwSetWindowSizeCallback(_window,
            [](GLFWwindow* window, int width, int height) {
                if (auto windowManager = static_cast<WindowsWindowManager*>(glfwGetWindowUserPointer(window))) {
                    LOGGER(verbose, "window new size: " << width << ", " << height);
                }
            }
        );

        glfwSetWindowRefreshCallback(_window,
            [](GLFWwindow* window) {
                if (auto windowManager = static_cast<WindowsWindowManager*>(glfwGetWindowUserPointer(window))) {
                    LOGGER(verbose, "window refresh");
                }
            }
        );

        glfwSetWindowFocusCallback(_window,
            [](GLFWwindow* window, int focused) {
                if (auto windowManager = static_cast<WindowsWindowManager*>(glfwGetWindowUserPointer(window))) {
                    LOGGER(verbose, "window focus state: " << (focused ? "focused" : "not focused"));
                }
            }
        );

        glfwSetWindowIconifyCallback(_window,
            [](GLFWwindow* window, int iconified) {
                if (auto windowManager = static_cast<WindowsWindowManager*>(glfwGetWindowUserPointer(window))) {
                    LOGGER(verbose, "window iconify state: " << (iconified ? "iconified" : "not iconified"));
                }
            }
        );

        glfwSetWindowMaximizeCallback(_window,
            [](GLFWwindow* window, int maximized) {
                if (auto windowManager = static_cast<WindowsWindowManager*>(glfwGetWindowUserPointer(window))) {
                    LOGGER(verbose, "window maximize state: " << (maximized ? "maximized" : "not maximized"));
                }
            }
        );

        glfwSetWindowContentScaleCallback(_window,
            [](GLFWwindow* window, float xscale, float yscale) {
                if (auto windowManager = static_cast<WindowsWindowManager*>(glfwGetWindowUserPointer(window))) {
                    LOGGER(verbose, "window new scale: " << xscale << ", " << yscale);
                }
            }
        );

        // error
        glfwSetErrorCallback(
            [](int error, const char* description) {
                LOGGER(error, "glfw error " << error << ": " << description);
            }
        );
    }

    void checkGLError() {
        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            LOGGER(error, "gl error occurred: " << err);
        }
    }

    void enableDebugOutput() {
        /*
        static const auto getDebugSourceString = [](GLenum source) {
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

        static const auto getDebugTypeString = [](GLenum type) {
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

        static const auto getDebugSeverityString = [](GLenum severity) {
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
        glDebugMessageCallback(
            [](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
                LOGGER(debug, "gl debug message:" << "\n"
                    << "severity: " << getDebugSeverityString(severity) << "\n"
                    << "type: " << getDebugTypeString(type) << "\n"
                    << "source: " << getDebugSourceString(source) << "\n"
                    << "id: " << id << "\n"
                    << "message: " << message);
            },
            nullptr
        );
        */
    }

    void disableDebugOutput() {
        glDisable(GL_DEBUG_OUTPUT);
    }

    unsigned int getClearMask() {
        return GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
    }

    GLFWwindow* _window{ nullptr };

};
