#pragma once

#include "WindowManager.h"
#include "Logger.h"
#include "State.h"

#include <string>
#include <vector>

class IControl {
public:
    IControl(WindowManager* windowManager) :
        _windowManager(windowManager),
        _mouseCurrentPosX(_windowManager->getMouseX()),
        _mouseCurrentPosY(_windowManager->getMouseY()) {}

    IControl(WindowManager* windowManager, State* state) :
        _windowManager(windowManager),
        _state(state),
        _mouseCurrentPosX(_windowManager->getMouseX()),
        _mouseCurrentPosY(_windowManager->getMouseY()) {}

    virtual ~IControl() = default;

    virtual void init() = 0;

    virtual void charCallback(GLFWwindow* window, unsigned int codepoint) = 0;

    virtual void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) = 0;

    virtual void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) = 0;

    virtual void framebufferSizeCallback(GLFWwindow* window, int width, int height) = 0;

    virtual void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) = 0;

    virtual void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) = 0;

    inline std::string getUserInput() {
        std::string userInputString;
        for (const auto& userInput : _userInputVector) {
            userInputString += userInput;
        }
        return userInputString;
    }

    inline double getMouseCurrentPosX() {
        return _mouseCurrentPosX;
    }

    inline double getMouseCurrentPosY() {
        return _mouseCurrentPosY;
    }

    size_t encode_utf8(char* s, unsigned int ch) {
        size_t count = 0;

        if (ch < 0x80)
            s[count++] = static_cast<char>(ch);
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

protected:
    WindowManager* _windowManager = nullptr;
    State* _state = nullptr;
    double _mouseCurrentPosX = 0;
    double _mouseCurrentPosY = 0;
    std::vector<std::string> _userInputVector{};
};
