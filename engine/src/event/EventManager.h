#pragma once

#include "EventType.h"
#include "InputEvent.h"
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

    void triggerInputEvent(KeyCode keycode, KeyAction keyAction) {
        for (auto& inputEvent : _inputEvents) {
            inputEvent->onEvent(keycode, keyAction);
        }
    }

    void triggerMouseButtonEvent(MouseButtonCode mouseButtonCode, MouseButtonAction mouseButtonAction) {
        for (auto& mouseButtonEvent : _mouseButtonEvents) {
            mouseButtonEvent->onEvent(mouseButtonCode, mouseButtonAction);
        }
    }

private:
    std::vector<std::shared_ptr<events::IInputEvent>> _inputEvents;
    std::vector<std::shared_ptr<events::IWindowEvent>> _windowEvents;
    std::vector<std::shared_ptr<events::IMouseButtonEvent>> _mouseButtonEvents;

};
