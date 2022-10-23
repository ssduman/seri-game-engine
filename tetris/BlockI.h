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
        LOGGER(info, "block I init succeeded");

        _layout = {
            { 0, 0, 0, 0 },
            { 1, 1, 1, 1 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
        };

        _row = 2;
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
        _block->setTexture("tetris-assets/textures/block-" + _blockIdentifier + ".png");
        _block->initMVP();
        _block->setPositions(_blockPositions);
        _block->init();
    }

private:
    void generateBlockPositions() {
        const auto offsetX = _tetrisProperties.totalCols / 2.0f;
        const auto offsetY = _tetrisProperties.totalRows - 2.0f;

        _blockPositions.reserve(4 * 4);

        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++) {
                if (_layout[y][x] == 1) {
                    const auto s1 = createSquarePosition(offsetX - x, offsetY - y);
                    _blockPositions.insert(_blockPositions.end(), s1.begin(), s1.end());
                }
            }
        }
    }

    std::string _blockIdentifier{ "I" };
};
