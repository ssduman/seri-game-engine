#pragma once

#include "../control/KeyCode.h"
#include "../control/KeyAction.h"

#include <memory>

namespace events {
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
}
