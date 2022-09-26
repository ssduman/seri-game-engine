#pragma once

#include "../engine/Logger.h"
#include "../engine/Entity.h"

#include "Board.h"
#include "Camera.h"
#include "TetrisProperties.h"

#include <vector>

class Tetris : public Entity {
public:
    Tetris(Camera* camera, TetrisProperties& tetrisProperties)
        : Entity(camera), _tetrisProperties(tetrisProperties), _board(camera, _tetrisProperties) {
        LOGGER(info, "tetris init succeeded");
    }

    ~Tetris() override {
        LOGGER(info, "tetris delete succeeded");
    }

    void init() override {
        _board.init();
    }

    void update() override {}

    void render() override {}

    void display() override {
        Object::display();

        _board.display();
    }

private:
    TetrisProperties& _tetrisProperties;
    Board _board;
};
