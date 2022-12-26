#pragma once

#include "../control/MouseButtonCode.h"
#include "../control/MouseButtonAction.h"

#include <memory>

namespace events {
    class IMouseButtonEvent {
    public:
        IMouseButtonEvent() = default;

        virtual ~IMouseButtonEvent() = default;

        virtual void onEvent(MouseButtonCode mouseButtonCode, MouseButtonAction mouseButtonAction) = 0;

    };

    template <typename F>
    class MouseButtonEvent : public IMouseButtonEvent {
    public:
        MouseButtonEvent(F f) : _f(std::move(f)) {};

        ~MouseButtonEvent() override = default;

        void onEvent(MouseButtonCode mouseButtonCode, MouseButtonAction mouseButtonAction) override {
            _f(mouseButtonCode, mouseButtonAction);
        }

    private:
        F _f;

    };

    template <typename F>
    inline std::shared_ptr<IMouseButtonEvent> makeMouseButtonEvent(F f) {
        return std::make_shared<MouseButtonEvent<F>>(std::move(f));
    }
}
