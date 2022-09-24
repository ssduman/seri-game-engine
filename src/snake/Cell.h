#pragma once

#include "../engine/Entity.h"

#include "SnakeMovement.h"

struct Cell {
    Cell(int x_, int y_, Entity* entity_, glm::vec2 position_, SnakeMovement direction_)
        : x{ x_ }, y{ y_ }, entity{ entity_ }, position{ position_ }, direction{ direction_ } {}

    int x;
    int y;
    Entity* entity;
    glm::vec2 position;
    SnakeMovement direction;
    SnakeMovement requestedDirection = SnakeMovement::noop;
};
