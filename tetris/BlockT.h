#pragma once

#include <random>
#include <vector>

#include "../engine/Entity.h"
#include "../engine/Logger.h"
#include "../engine/Polygon.h"
#include "../engine/Rectangle.h"
#include "../engine/Triangle.h"
#include "Camera.h"
#include "IBlock.h"
#include "TetrisProperties.h"

class BlockT : public IBlock {
public:
    BlockT(Camera* camera, TetrisProperties& tetrisProperties) : IBlock(camera, tetrisProperties) {
        LOGGER(info, "block T init succeeded");

        _layout = {
            { 0, 0, 0, 0 },
            { 1, 1, 1, 0 },
            { 0, 1, 0, 0 },
            { 0, 0, 0, 0 },
        };

        _row = 3;
    }

    ~BlockT() override {
        delete _block;

        LOGGER(info, "block T delete succeeded");
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

    std::string _blockIdentifier{ "T" };
};
