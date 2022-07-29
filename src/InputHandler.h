#pragma once

#include "ICommand.h"

class InputHandler {
public:
    InputHandler() = default;
    ~InputHandler() = default;

    ICommand* handleInput(int key, int scancode, int action, int mods) {
        if (isPressed(key, action, GLFW_KEY_X)) {
            return _x;
        }
        if (isPressed(key, action, GLFW_KEY_Y)) {
            return _y;
        }
        if (isPressed(key, action, GLFW_KEY_UP)) {
            return _arrowUp;
        }
        if (isPressed(key, action, GLFW_KEY_DOWN)) {
            return _arrowDown;
        }
        if (isPressed(key, action, GLFW_KEY_LEFT)) {
            return _arrowLeft;
        }
        if (isPressed(key, action, GLFW_KEY_RIGHT)) {
            return _arrowRight;
        }
        if (isPressed(key, action, GLFW_KEY_ENTER)) {
            return _enter;
        }
        if (isPressed(key, action, GLFW_KEY_SPACE)) {
            return _space;
        }
        if (isPressed(key, action, GLFW_KEY_ESCAPE)) {
            return _escape;
        }

        if (action == GLFW_PRESS) {
            return _press;
        }
        if (action == GLFW_REPEAT) {
            return _repeat;
        }
        if (action == GLFW_RELEASE) {
            return _release;
        }

        return nullptr;
    }

private:
    bool isPressed(int key, int action, int target) {
        if ((key == target) && (action == GLFW_PRESS)) {
            return true;
        }
        return false;
    }

    bool isReleased(int key, int action, int target) {
        if ((key == target) && (action == GLFW_RELEASE)) {
            return true;
        }
        return false;
    }

    bool isRepeated(int key, int action, int target) {
        if ((key == target) && (action == GLFW_REPEAT)) {
            return true;
        }
        return false;
    }

    ICommand* _x;
    ICommand* _y;
    ICommand* _enter;
    ICommand* _space;
    ICommand* _escape;
    ICommand* _arrowUp;
    ICommand* _arrowDown;
    ICommand* _arrowLeft;
    ICommand* _arrowRight;

    ICommand* _press;
    ICommand* _repeat;
    ICommand* _release;

};
