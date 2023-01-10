#pragma once

#include "EventData.h"
#include "EventManager.h"

#include <memory>

namespace events {
    class IEventCallback {
    public:
        IEventCallback() = default;

        virtual ~IEventCallback() = default;

        virtual void onEvent(IEventData& data) = 0;

    };

    template <typename F>
    class EventCallback : public IEventCallback {
    public:
        EventCallback(F f) : _f(std::move(f)) {};

        ~EventCallback() override = default;

        void onEvent(IEventData& data) override {
            _f(data);
        }

    private:
        F _f;

    };

    template <typename F>
    inline std::shared_ptr<IEventCallback> makeEventCallback(F f) {
        return std::make_shared<EventCallback<F>>(std::move(f));
    }
}
