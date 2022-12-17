#pragma once

#include "../control/KeyCode.h"

#include <memory>
#include <vector>

class IInputEvent {
public:
    IInputEvent() = default;

    virtual ~IInputEvent() = default;

    virtual void onEvent(KeyCode keycode) = 0;

};

template <typename F>
class InputEvent : public IInputEvent {
public:
    InputEvent(F f) : _f(std::move(f)) {};

    ~InputEvent() override = default;

    void onEvent(KeyCode keycode) override {
        _f(std::move(keycode));
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

    void triggerInputEvent(KeyCode keycode) {
        for (auto& inputEvent : _inputEvents) {
            inputEvent->onEvent(std::move(keycode));
        }
    }

private:
    std::vector<std::shared_ptr<IInputEvent>> _inputEvents;

};
