#pragma once

#include "ICommand.h"

class InputHandler {
public:
    InputHandler(Camera* camera) : _camera(camera) {}
    ~InputHandler() = default;

    ICommand* handleInput(int key, int scancode, int action, int mods) {
        if (isPressing(key, action, GLFW_KEY_W)) {
            _camera->handleInput(CameraMovement::FORWARD);
        }
        if (isPressing(key, action, GLFW_KEY_S)) {
            _camera->handleInput(CameraMovement::BACKWARD);
        }
        if (isPressing(key, action, GLFW_KEY_A)) {
            _camera->handleInput(CameraMovement::LEFT);
        }
        if (isPressing(key, action, GLFW_KEY_D)) {
            _camera->handleInput(CameraMovement::RIGHT);
        }

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

    bool isPressing(int key, int action, int target) {
        if ((key == target) && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
            return true;
        }
        return false;
    }

    Camera* _camera;

    ICommand* _x = nullptr;
    ICommand* _y = nullptr;
    ICommand* _enter = nullptr;
    ICommand* _space = nullptr;
    ICommand* _escape = nullptr;
    ICommand* _arrowUp = nullptr;
    ICommand* _arrowDown = nullptr;
    ICommand* _arrowLeft = nullptr;
    ICommand* _arrowRight = nullptr;

    ICommand* _press = nullptr;
    ICommand* _repeat = nullptr;
    ICommand* _release = nullptr;
};
