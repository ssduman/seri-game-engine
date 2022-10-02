#pragma once

#include "../engine/Entity.h"
#include "../engine/Logger.h"
#include "../engine/Polygon.h"
#include "../engine/Triangle.h"
#include "../engine/Rectangle.h"

#include "Camera.h"
#include "IBlock.h"
#include "TetrisProperties.h"

#include <vector>
#include <random>

class BlockJ : public IBlock {
public:
    BlockJ(Camera* camera, TetrisProperties& tetrisProperties) : IBlock(camera, tetrisProperties) {
        LOGGER(info, "block J init succeeded");
    }

    ~BlockJ() override {
        delete _block;

        LOGGER(info, "block J delete succeeded");
    }

    void init() override {
        generateBlock();
    }

    void update() override {}

    void render() override {}

    void display() override {
        Object::display();
        _block->display();
    }

    void generateBlock() override {
        generateBlockPositions();

        _block = new Polygon(_camera);
        _block->initShader("tetris-assets/shaders/tetris_vs.shader", "tetris-assets/shaders/tetris_fs.shader");
        _block->setDrawMode(GL_TRIANGLE_STRIP);
        _block->setTexture("tetris-assets/textures/block-" + _blockIdentifier + ".png", _blockPositions);
        _block->initMVP();
        _block->setPositionsVec2(_blockPositions);
        _block->init();
    }

private:
    void generateBlockPositions() {
        const auto x = _tetrisProperties.totalCols / 2.0f - 1.0f;
        const auto y = _tetrisProperties.totalRows - 2.0f;

        const auto s1 = createSquarePosition(x + 0.0f, y + 0.0f);
        const auto s2 = createSquarePosition(x + 0.0f, y - 1.0f);
        const auto s3 = createSquarePosition(x + 0.0f, y - 2.0f);
        const auto s4 = createSquarePosition(x - 1.0f, y - 2.0f);

        _blockPositions.reserve(4 * 4);
        _blockPositions.insert(_blockPositions.end(), s1.begin(), s1.end());
        _blockPositions.insert(_blockPositions.end(), s2.begin(), s2.end());
        _blockPositions.insert(_blockPositions.end(), s3.begin(), s3.end());
        _blockPositions.insert(_blockPositions.end(), s4.begin(), s4.end());
    }

    std::string _blockIdentifier{ "J" };

};
