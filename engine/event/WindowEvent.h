#pragma once

#include <memory>
#include <vector>

namespace events {
    class IWindowEvent {
    public:
        IWindowEvent() = default;

        virtual ~IWindowEvent() = default;

        virtual void onEvent() = 0;

    };

    template <typename F>
    class WindowEvent : public IWindowEvent {
    public:
        WindowEvent(F f) : _f(std::move(f)) {};

        ~WindowEvent() override = default;

        void onEvent() override {
            _f();
        }

    private:
        F _f;

    };

    template <typename F>
    inline std::shared_ptr<IWindowEvent> makeWindowEvent(F f) {
        return std::make_shared<WindowEvent<F>>(std::move(f));
    }
}
