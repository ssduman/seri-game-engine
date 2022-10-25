#pragma once

#include "../engine/Entity.h"
#include "../engine/Logger.h"
#include "../engine/Polygon.h"

#include "Camera.h"
#include "IBlock.h"
#include "TetrisProperties.h"

#include <random>
#include <vector>

class BlockI : public IBlock {
public:
    BlockI(Camera* camera, TetrisProperties& tetrisProperties) : IBlock(camera, tetrisProperties) {
        _blockIdentifier = "I";
        _layout = {
            { 0, 0, 0, 0 },
            { 1, 1, 1, 1 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
        };
        _row = 2;

        LOGGER(info, "block I init succeeded");
    }

    ~BlockI() override {
        delete _block;

        LOGGER(info, "block I delete succeeded");
    }

    void init() override {
        generateBlock();
    }

    void update() override {}

    void display() override {
        Object::display();
        _block->display();
    }

    void generateBlock() override {
        generateBlockPositions();

        _block = new Polygon(_camera);
        _block->initShader(vertexShader, fragmentShader, /*readFromFile*/ false);
        _block->setDrawMode(GL_TRIANGLES);
        _block->setTexture("tetris-assets/textures/block-" + _blockIdentifier + ".png", _blockPositions);
        _block->initMVP();
        _block->setPositions(_blockPositions);
        _block->init();
    }

};
