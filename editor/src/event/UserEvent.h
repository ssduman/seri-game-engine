#pragma once

#include <core/Seri.h>

enum class UserEventType {
    test,
    unknown,
};

inline const char* toString(UserEventType userEventType) {
    switch (userEventType) {
        case UserEventType::test: return "test";
        case UserEventType::unknown: return "unknown";
        default: return "";
    }
}

struct IUserEventData : public IEventData {
    IUserEventData(UserEventType type_): IEventData(EventType::user), userEventType(type_) {}

    ~IUserEventData() override = default;

    UserEventType userEventType = UserEventType::unknown;
};

struct UserTestEventData : public IUserEventData {
    UserTestEventData(int data_) : IUserEventData(UserEventType::test), data(data_) {}

    ~UserTestEventData() override = default;

    std::string toString() override {
        std::stringstream ss;
        ss << ::toString(eventType) << "/" << ::toString(userEventType) << ": {"
            << "'data': " << data
            << "}";

        return ss.str();
    }

    int data;
};
