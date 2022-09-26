#pragma once

#include "../engine/Logger.h"
#include "../engine/Entity.h"

#include "Camera.h"
#include "TetrisProperties.h"

#include <vector>

class Tetris : public Entity {
public:
    Tetris(Camera* camera, TetrisProperties& tetrisProperties) : Entity(camera), _tetrisProperties(tetrisProperties) {
        LOGGER(info, "tetris init succeeded");
    }

    ~Tetris() override {
        LOGGER(info, "tetris delete succeeded");
    }

    void init() override {}

    void update() override {}

    void render() override {}

    void display() override {}

private:
    TetrisProperties& _tetrisProperties;

};
