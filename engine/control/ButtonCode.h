#pragma once

#include <GLFW/glfw3.h>

enum class ButtonCode {
    button_1 = GLFW_MOUSE_BUTTON_1,
    button_2 = GLFW_MOUSE_BUTTON_2,
    button_3 = GLFW_MOUSE_BUTTON_3,
    button_4 = GLFW_MOUSE_BUTTON_4,
    button_5 = GLFW_MOUSE_BUTTON_5,
    button_6 = GLFW_MOUSE_BUTTON_6,
    button_7 = GLFW_MOUSE_BUTTON_7,
    button_8 = GLFW_MOUSE_BUTTON_8,
    button_last = GLFW_MOUSE_BUTTON_LAST,
    button_left = GLFW_MOUSE_BUTTON_LEFT,
    button_right = GLFW_MOUSE_BUTTON_RIGHT,
    button_middle = GLFW_MOUSE_BUTTON_MIDDLE,
};
