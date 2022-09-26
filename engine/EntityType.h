#pragma once

#include <string>

enum class EntityType {
    POINT,
    LINE,
    TRIANGLE,
    RECTANGLE,
    CIRCLE,
    CUBE,
    UNKNOWN,
};

inline std::string toString(EntityType entityType) {
    switch (entityType) {
        case EntityType::POINT:
            return "point";
        case EntityType::LINE:
            return "line";
        case EntityType::TRIANGLE:
            return "triangle";
        case EntityType::RECTANGLE:
            return "rectangle";
        case EntityType::CIRCLE:
            return "circle";
        case EntityType::CUBE:
            return "cube";
        case EntityType::UNKNOWN:
            return "unknown";
        default:
            return "unknown";
    }
}
