#pragma once

#include "EventType.h"
#include "InputEvent.h"
#include "WindowEvent.h"

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

private:
    std::vector<std::shared_ptr<events::IInputEvent>> _inputEvents;
    std::vector<std::shared_ptr<events::IWindowEvent>> _windowEvents;

};
