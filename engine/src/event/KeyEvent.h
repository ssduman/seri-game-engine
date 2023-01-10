#pragma once

#include "../input/KeyCode.h"
#include "../input/InputAction.h"
#include "../input/InputModifier.h"

#include <memory>

namespace events {
    class IKeyEvent {
    public:
        IKeyEvent() = default;

        virtual ~IKeyEvent() = default;

        virtual void onEvent(KeyCode key, int scancode, InputAction action, InputModifier mods) = 0;

    };

    template <typename F>
    class KeyEvent : public IKeyEvent {
    public:
        KeyEvent(F f) : _f(std::move(f)) {};

        ~KeyEvent() override = default;

        void onEvent(KeyCode key, int scancode, InputAction action, InputModifier mods) override {
            _f(key, scancode, action, mods);
        }

    private:
        F _f;

    };

    template <typename F>
    inline std::shared_ptr<IKeyEvent> makeKeyEvent(F f) {
        return std::make_shared<KeyEvent<F>>(std::move(f));
    }
}
