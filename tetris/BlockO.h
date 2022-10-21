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

class BlockO : public IBlock {
public:
    BlockO(Camera* camera, TetrisProperties& tetrisProperties) : IBlock(camera, tetrisProperties) {
        LOGGER(info, "block O init succeeded");
    }

    ~BlockO() override {
        delete _block;

        LOGGER(info, "block O delete succeeded");
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
        const auto x = _tetrisProperties.totalCols / 2.0f - 1.0f;
        const auto y = _tetrisProperties.totalRows - 2.0f;

        const auto s1 = createSquarePosition(x + 0.0f, y + 1.0f);
        const auto s2 = createSquarePosition(x + 1.0f, y + 1.0f);
        const auto s3 = createSquarePosition(x + 0.0f, y + 0.0f);
        const auto s4 = createSquarePosition(x + 1.0f, y + 0.0f);

        _blockPositions.reserve(4 * 4);
        _blockPositions.insert(_blockPositions.end(), s1.begin(), s1.end());
        _blockPositions.insert(_blockPositions.end(), s2.begin(), s2.end());
        _blockPositions.insert(_blockPositions.end(), s3.begin(), s3.end());
        _blockPositions.insert(_blockPositions.end(), s4.begin(), s4.end());
    }

    std::string _blockIdentifier{ "O" };
};
