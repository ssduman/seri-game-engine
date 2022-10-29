#pragma once

#include <string>

enum class EntityType {
    point,
    line,
    triangle,
    rectangle,
    circle,
    cube,
    polygon,
    unknown,
};

inline std::string toString(EntityType entityType) {
    switch (entityType) {
        case EntityType::point:
            return "point";
        case EntityType::line:
            return "line";
        case EntityType::triangle:
            return "triangle";
        case EntityType::rectangle:
            return "rectangle";
        case EntityType::circle:
            return "circle";
        case EntityType::cube:
            return "cube";
        case EntityType::polygon:
            return "polygon";
        case EntityType::unknown:
            return "unknown";
    }
}
