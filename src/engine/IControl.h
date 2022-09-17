#pragma once

#include "WindowManager.h"
#include "Logger.h"
#include "State.h"

#include <string>
#include <vector>

class IControl {
public:
    IControl(WindowManager* windowManager) : _windowManager(windowManager) {}

    IControl(WindowManager* windowManager, State* state) : _windowManager(windowManager), _state(state) {}

    virtual ~IControl() = default;

    virtual void init() = 0;

    virtual void charCallback(GLFWwindow* window, unsigned int codepoint) = 0;

    virtual void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) = 0;

    virtual void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) = 0;

    virtual void framebufferSizeCallback(GLFWwindow* window, int width, int height) = 0;

    virtual void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) = 0;

    virtual void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) = 0;

    void addUserInput(std::string& str) {
        _userInputString += str;
        _userInputVector.emplace_back(str);
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

    void clearUserInput() {
        _userInputString.clear();
        _userInputVector.clear();
    }

    const std::string& getUserInput() {
        return _userInputString;
    }

    int getUserInputSize() {
        return static_cast<int>(_userInputVector.size());
    }

    const std::vector<std::string>& getUserInputVector() {
        return _userInputVector;
    }

    double getMouseCurrentPosX() {
        return _windowManager->getMouseX();
    }

    double getMouseCurrentPosY() {
        return _windowManager->getMouseY();
    }

    std::string encodeUTF8(const unsigned int codepoint) {
        std::string str;

        if (codepoint < 0x80) {
            str += static_cast<char>(codepoint);
        } else if (codepoint < 0x800) {
            str += (codepoint >> 6) | 0xc0;
            str += (codepoint & 0x3f) | 0x80;
        } else if (codepoint < 0x10000) {
            str += (codepoint >> 12) | 0xe0;
            str += ((codepoint >> 6) & 0x3f) | 0x80;
            str += (codepoint & 0x3f) | 0x80;
        } else if (codepoint < 0x110000) {
            str += (codepoint >> 18) | 0xf0;
            str += ((codepoint >> 12) & 0x3f) | 0x80;
            str += ((codepoint >> 6) & 0x3f) | 0x80;
            str += (codepoint & 0x3f) | 0x80;
        }

        return str;
    }

protected:
    WindowManager* _windowManager = nullptr;
    State* _state = nullptr;
    double _mouseCurrentPosX = 0;
    double _mouseCurrentPosY = 0;
    std::string _userInputString;
    std::vector<std::string> _userInputVector;
};
