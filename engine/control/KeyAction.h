#pragma once

#include <GLFW/glfw3.h>

enum class KeyAction : int {
    press = GLFW_PRESS,
    repeat = GLFW_REPEAT,
    release = GLFW_RELEASE,
};

inline const char* toString(KeyAction keyAction) {
    switch (keyAction) {
        case KeyAction::press: return "press";
        case KeyAction::repeat: return "repeat";
        case KeyAction::release: return "release";
        default: return "";
    }
}
