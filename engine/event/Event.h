#pragma once

#include "../control/KeyCode.h"
#include "../control/KeyAction.h"

#include <memory>
#include <vector>

class IInputEvent {
public:
    IInputEvent() = default;

    virtual ~IInputEvent() = default;

    virtual void onEvent(KeyCode keycode, KeyAction keyAction) = 0;

};

template <typename F>
class InputEvent : public IInputEvent {
public:
    InputEvent(F f) : _f(std::move(f)) {};

    ~InputEvent() override = default;

    void onEvent(KeyCode keycode, KeyAction keyAction) override {
        _f(keycode, keyAction);
    }

private:
    F _f;

};

template <typename F>
inline std::shared_ptr<IInputEvent> makeInputEvent(F f) {
    return std::make_shared<InputEvent<F>>(std::move(f));
}

enum class EventType {
    window,
    key,
    mouse,
    app,
};

class Event {
public:
    template <typename F>
    void addInputEventListener(F f) {
        _inputEvents.emplace_back(makeInputEvent(std::move(f)));
    }

    void triggerInputEvent(KeyCode keycode, KeyAction keyAction) {
        for (auto& inputEvent : _inputEvents) {
            inputEvent->onEvent(keycode, keyAction);
        }
    }

private:
    std::vector<std::shared_ptr<IInputEvent>> _inputEvents;

};
