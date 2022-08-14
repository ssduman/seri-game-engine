#pragma once
#pragma warning(disable: 4100)
#pragma warning(disable: 4244)

#include "Window.h"
#include "IControl.h"
#include "InputHandler.h"

class Control : public IControl {
public:
    Control(Window window, Camera* camera) : IControl(window), _camera(camera), _inputHandler(camera) {
        glfwSetWindowUserPointer(_window.getWindow(), static_cast<void*>(this));
    }

    virtual ~Control() {}

    void init() override {
        glfwSetCharCallback(_window.getWindow(),
            [](GLFWwindow* window, unsigned int codepoint) {
                if (auto control = static_cast<Control*>(glfwGetWindowUserPointer(window))) {
                    control->charCallback(window, codepoint);
                }
            }
        );

        glfwSetMouseButtonCallback(_window.getWindow(),
            [](GLFWwindow* window, int button, int action, int mods) {
                if (auto control = static_cast<Control*>(glfwGetWindowUserPointer(window))) {
                    control->mouseButtonCallback(window, button, action, mods);
                }
            }
        );

        glfwSetScrollCallback(_window.getWindow(),
            [](GLFWwindow* window, double xoffset, double yoffset) {
                if (auto control = static_cast<Control*>(glfwGetWindowUserPointer(window))) {
                    control->scrollCallback(window, xoffset, yoffset);
                }
            }
        );

        glfwSetFramebufferSizeCallback(_window.getWindow(),
            [](GLFWwindow* window, int width, int height) {
                if (auto control = static_cast<Control*>(glfwGetWindowUserPointer(window))) {
                    control->framebufferSizeCallback(window, width, height);
                }
            }
        );

        glfwSetCursorPosCallback(_window.getWindow(),
            [](GLFWwindow* window, double xpos, double ypos) {
                if (auto control = static_cast<Control*>(glfwGetWindowUserPointer(window))) {
                    control->cursorPosCallback(window, xpos, ypos);
                }
            }
        );

        glfwSetKeyCallback(_window.getWindow(),
            [](GLFWwindow* window, int key, int scancode, int action, int mods) {
                if (auto control = static_cast<Control*>(glfwGetWindowUserPointer(window))) {
                    control->keyCallback(window, key, scancode, action, mods);
                }
            }
        );
    }

    void charCallback(GLFWwindow* window, unsigned int codepoint) override {
        char string[5] = "";
        encode_utf8(string, codepoint);
        userInputVector.push_back(std::string(string));
    }

    void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) override {
        //double deltaX = xpos - mouseCurrentPosX;
        //double deltaY = ypos - mouseCurrentPosY;

        mouseCurrentPosX = xpos;
        mouseCurrentPosY = ypos;
    }

    void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) override {}

    void framebufferSizeCallback(GLFWwindow* window, int width, int height) override {
        _window.getWidth() = width;
        _window.getHeight() = height;
        glViewport(0, 0, width, height);
    }

    void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) override {
        if ((button == GLFW_MOUSE_BUTTON_LEFT) && (action == GLFW_PRESS)) {
        }
    }

    void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) override {
        ICommand* command = _inputHandler.handleInput(key, scancode, action, mods);
        if (command) {
            command->execute();
        }

        if ((key == GLFW_KEY_ENTER) && (action == GLFW_PRESS)) {
            userInputVector.clear();
        } else if ((key == GLFW_KEY_BACKSPACE) && (action == GLFW_PRESS)) {
            if (userInputVector.size() > 0) {
                userInputVector.pop_back();
            }
        } else if ((key == GLFW_KEY_ESCAPE) && (action == GLFW_PRESS)) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        if (action == GLFW_RELEASE) {
            std::cout << "userInput: " << getUserInput() << std::endl;
        }
    }

    void setInputHandler(InputHandler inputHandler) {
        _inputHandler = inputHandler;
    }

private:
    size_t encode_utf8(char* s, unsigned int ch) {
        size_t count = 0;

        if (ch < 0x80)
            s[count++] = (char)ch;
        else if (ch < 0x800) {
            s[count++] = (ch >> 6) | 0xc0;
            s[count++] = (ch & 0x3f) | 0x80;
        } else if (ch < 0x10000) {
            s[count++] = (ch >> 12) | 0xe0;
            s[count++] = ((ch >> 6) & 0x3f) | 0x80;
            s[count++] = (ch & 0x3f) | 0x80;
        } else if (ch < 0x110000) {
            s[count++] = (ch >> 18) | 0xf0;
            s[count++] = ((ch >> 12) & 0x3f) | 0x80;
            s[count++] = ((ch >> 6) & 0x3f) | 0x80;
            s[count++] = (ch & 0x3f) | 0x80;
        }

        return count;
    }

    InputHandler _inputHandler;
    Camera* _camera;
};
