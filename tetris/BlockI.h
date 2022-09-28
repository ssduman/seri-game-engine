#pragma once

#include "../engine/Entity.h"
#include "../engine/Logger.h"
#include "../engine/Rectangle.h"

#include "Camera.h"
#include "IBlock.h"
#include "TetrisProperties.h"

#include <vector>
#include <random>

class BlockI : public IBlock {
public:
    BlockI(Camera* camera, TetrisProperties& tetrisProperties) : IBlock(camera, tetrisProperties) {
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

    void render() override {}

    void display() override {
        Object::display();
        _block->display();
    }

    void generateBlock() override {
        generateBlockPositions();

        _block = new Rectangle(_camera);
        _block->initShader("tetris-assets/shaders/tetris_vs.shader", "tetris-assets/shaders/tetris_fs.shader");
        _block->setTexture("tetris-assets/textures/block-" + _blockIdentifier + ".png");
        _block->initMVP();
        _block->setPositionsVec2(_blockPositions);
        _block->init();
    }

private:
    void generateBlockPositions() {
        const auto x = _tetrisProperties.totalCols / 2.0f - 1.0f;
        const auto y = _tetrisProperties.totalRows - 2.0f;

        const auto interval = _tetrisProperties.interval;
        const auto d1 = (interval * 0.0f) / 2.0f;
        const auto d2 = (interval * 2.0f) / 2.0f;
        _blockPositions = {
            { (x - 1) * interval + d1, (y - 1) * interval + d1 },
            { (x - 1) * interval + d1, (y + 1) * interval + d2 },
            { (x + 1) * interval + d2, (y + 1) * interval + d2 },
            { (x + 1) * interval + d2, (y - 1) * interval + d1 },
        };
    }

    std::string _blockIdentifier{ "I" };

};
