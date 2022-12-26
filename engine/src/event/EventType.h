#pragma once

enum class EventType {
    window,
    key,
    input,
    mouse,
    app,
};

inline const char* toString(EventType eventType) {
    switch (eventType) {
        case EventType::window: return "window";
        case EventType::key: return "key";
        case EventType::input: return "input";
        case EventType::mouse: return "mouse";
        case EventType::app: return "app";
        default: return "";
    }
}
