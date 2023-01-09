#pragma once

#include "../control/KeyCode.h"
#include "../control/InputAction.h"
#include "../control/InputModifier.h"
#include "../control/MouseButtonCode.h"

class IEvent {
public:
    virtual ~IEvent() = default;

    virtual void onCharEvent(unsigned int codepoint) {}

    virtual void onKeyEvent(KeyCode key, int scancode, InputAction action, InputModifier mods) {}

    virtual void onScrollEvent(double xoffset, double yoffset) {}

    virtual void onMousePositionEvent(double xpos, double ypos) {}

    virtual void onMouseButtonEvent(MouseButtonCode button, InputAction action, InputModifier mods) {}

    virtual void onFramebufferSizeEvent(int width, int height) {}

private:

};
