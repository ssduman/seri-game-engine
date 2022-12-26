#pragma once

#include <GLFW/glfw3.h>

enum class MouseButtonAction : int {
    press = GLFW_PRESS,
    repeat = GLFW_REPEAT,
    release = GLFW_RELEASE,
};

inline const char* toString(MouseButtonAction mouseButtonAction) {
    switch (mouseButtonAction) {
        case MouseButtonAction::press: return "press";
        case MouseButtonAction::repeat: return "repeat";
        case MouseButtonAction::release: return "release";
        default: return "";
    }
}
