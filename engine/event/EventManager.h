#pragma once

#include "EventType.h"
#include "InputEvent.h"
#include "WindowEvent.h"

class EventManager {
public:
    template <typename F>
    void addEventListener(EventType eventType, F f) {
        switch (eventType) {
            case EventType::window:
                _windowEvents.emplace_back(events::makeWindowEvent(std::move(f)));
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

    void triggerInputEvent(KeyCode keycode, KeyAction keyAction) {
        for (auto& inputEvent : _inputEvents) {
            inputEvent->onEvent(keycode, keyAction);
        }
    }

private:
    std::vector<std::shared_ptr<events::IInputEvent>> _inputEvents;
    std::vector<std::shared_ptr<events::IWindowEvent>> _windowEvents;

};
