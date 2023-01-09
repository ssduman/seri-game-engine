#pragma once

#include "KeyEvent.h"
#include "EventType.h"
#include "WindowEvent.h"
#include "MouseButtonEvent.h"

class EventManager {
public:
    template <typename F>
    void addInputEventListener(F f) {
        _inputEvents.emplace_back(events::makeInputEvent(std::move(f)));
    }

    template <typename F>
    void addWindowEventListener(F f) {
        _windowEvents.emplace_back(events::makeWindowEvent(std::move(f)));
    }

    template <typename F>
    void addMouseButtonEventListener(F f) {
        _mouseButtonEvents.emplace_back(events::makeMouseButtonEvent(std::move(f)));
    }

    void triggerWindowEvent() {
        for (auto& windowEvent : _windowEvents) {
            windowEvent->onEvent();
        }
    }

    void triggerKeyEvent(KeyCode key, int scancode, InputAction action, InputModifier mods) {
        for (auto& inputEvent : _keyEvents) {
            inputEvent->onEvent(key, scancode, action, mods);
        }
    }

    void triggerMouseButtonEvent(MouseButtonCode button, InputAction action, InputModifier mods) {
        for (auto& mouseButtonEvent : _mouseButtonEvents) {
            mouseButtonEvent->onEvent(button, action, mods);
        }
    }

private:
    std::vector<std::shared_ptr<events::IKeyEvent>> _keyEvents;
    std::vector<std::shared_ptr<events::IWindowEvent>> _windowEvents;
    std::vector<std::shared_ptr<events::IMouseButtonEvent>> _mouseButtonEvents;

};
