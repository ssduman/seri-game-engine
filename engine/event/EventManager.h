#pragma once

#include "InputEvent.h"

enum class EventType {
    window,
    key,
    input,
    mouse,
    app,
};

class EventManager {
public:
    template <typename F>
    void addEventListener(EventType eventType, F f) {
        switch (eventType) {
            case EventType::window:
                break;
            case EventType::key:
                break;
            case EventType::input:
                _inputEvents.emplace_back(events::makeInputEvent(std::move(f)));
                break;
            case EventType::mouse:
                break;
            case EventType::app:
                break;
            default:
                break;
        }
    }

    template <typename F>
    void addInputEventListener(F f) {
        _inputEvents.emplace_back(events::makeInputEvent(std::move(f)));
    }

    void triggerInputEvent(KeyCode keycode, KeyAction keyAction) {
        for (auto& inputEvent : _inputEvents) {
            inputEvent->onEvent(keycode, keyAction);
        }
    }

private:
    std::vector<std::shared_ptr<events::IInputEvent>> _inputEvents;

};
