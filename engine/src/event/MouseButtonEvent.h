#pragma once

#include "../control/InputAction.h"
#include "../control/InputModifier.h"
#include "../control/MouseButtonCode.h"

#include <memory>

namespace events {
    class IMouseButtonEvent {
    public:
        IMouseButtonEvent() = default;

        virtual ~IMouseButtonEvent() = default;

        virtual void onEvent(MouseButtonCode button, InputAction action, InputModifier mods) = 0;

    };

    template <typename F>
    class MouseButtonEvent : public IMouseButtonEvent {
    public:
        MouseButtonEvent(F f) : _f(std::move(f)) {};

        ~MouseButtonEvent() override = default;

        void onEvent(MouseButtonCode button, InputAction action, InputModifier mods) override {
            _f(button, action, mods);
        }

    private:
        F _f;

    };

    template <typename F>
    inline std::shared_ptr<IMouseButtonEvent> makeMouseButtonEvent(F f) {
        return std::make_shared<MouseButtonEvent<F>>(std::move(f));
    }
}
