#pragma once

#include "State.h"
#include "Logger.h"
#include "WindowManager.h"

#include <memory>
#include <string>
#include <vector>

class IControl {
public:
    IControl(std::shared_ptr<WindowManager> windowManager) : _windowManager(windowManager) {}

    IControl(std::shared_ptr<WindowManager> windowManager, std::shared_ptr<State> state) : _windowManager(windowManager), _state(state) {}

    virtual ~IControl() = default;

    virtual void init() {
        glfwSetCharCallback(_windowManager->getWindow(),
            [](GLFWwindow* window, unsigned int codepoint) {
                if (auto control = static_cast<IControl*>(glfwGetWindowUserPointer(window))) {
                    control->charCallback(window, codepoint);
                }
            }
        );

        glfwSetMouseButtonCallback(_windowManager->getWindow(),
            [](GLFWwindow* window, int button, int action, int mods) {
                if (auto control = static_cast<IControl*>(glfwGetWindowUserPointer(window))) {
                    control->mouseButtonCallback(window, button, action, mods);
                }
            }
        );

        glfwSetScrollCallback(_windowManager->getWindow(),
            [](GLFWwindow* window, double xoffset, double yoffset) {
                if (auto control = static_cast<IControl*>(glfwGetWindowUserPointer(window))) {
                    control->scrollCallback(window, xoffset, yoffset);
                }
            }
        );

        glfwSetFramebufferSizeCallback(_windowManager->getWindow(),
            [](GLFWwindow* window, int width, int height) {
                if (auto control = static_cast<IControl*>(glfwGetWindowUserPointer(window))) {
                    control->framebufferSizeCallback(window, width, height);
                }
            }
        );

        glfwSetCursorPosCallback(_windowManager->getWindow(),
            [](GLFWwindow* window, double xpos, double ypos) {
                if (auto control = static_cast<IControl*>(glfwGetWindowUserPointer(window))) {
                    control->cursorPosCallback(window, xpos, ypos);
                }
            }
        );

        glfwSetKeyCallback(_windowManager->getWindow(),
            [](GLFWwindow* window, int key, int scancode, int action, int mods) {
                if (auto control = static_cast<IControl*>(glfwGetWindowUserPointer(window))) {
                    control->keyCallback(window, key, scancode, action, mods);
                }
            }
        );
    }

    virtual void charCallback(GLFWwindow* window, unsigned int codepoint) = 0;

    virtual void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) = 0;

    virtual void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) = 0;

    virtual void framebufferSizeCallback(GLFWwindow* window, int width, int height) = 0;

    virtual void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) = 0;

    virtual void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) = 0;

    void clearUserInput() {
        _userInputString.clear();
        _userInputVector.clear();
    }

    int getUserInputSize() {
        return static_cast<int>(_userInputVector.size());
    }

    void deleteLastUserInput() {
        if (!_userInputVector.empty()) {
            _userInputVector.pop_back();
        }
        _userInputString.clear();
        for (const auto& str : _userInputVector) {
            _userInputString += str;
        }
    }

    const std::string& getUserInput() {
        return _userInputString;
    }

    void addUserInput(const std::string& str) {
        _userInputString += str;
        _userInputVector.emplace_back(str);
    }

    const std::vector<std::string>& getUserInputVector() {
        return _userInputVector;
    }

    std::string encodeUTF8(const unsigned int codepoint) {
        std::string str;

        if (codepoint < 0x80) {
            str += static_cast<char>(codepoint);
        }
        else if (codepoint < 0x800) {
            str += (codepoint >> 6) | 0xc0;
            str += (codepoint & 0x3f) | 0x80;
        }
        else if (codepoint < 0x10000) {
            str += (codepoint >> 12) | 0xe0;
            str += ((codepoint >> 6) & 0x3f) | 0x80;
            str += (codepoint & 0x3f) | 0x80;
        }
        else if (codepoint < 0x110000) {
            str += (codepoint >> 18) | 0xf0;
            str += ((codepoint >> 12) & 0x3f) | 0x80;
            str += ((codepoint >> 6) & 0x3f) | 0x80;
            str += (codepoint & 0x3f) | 0x80;
        }

        return str;
    }

protected:
    std::shared_ptr<WindowManager> _windowManager;
    std::shared_ptr<State> _state;

    double _mouseCurrentPosX{ 0 };
    double _mouseCurrentPosY{ 0 };

    std::string _userInputString;
    std::vector<std::string> _userInputVector;

};
