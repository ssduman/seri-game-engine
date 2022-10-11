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

    void render() override {
        _block->getShader().setVec2("u_position", glm::vec2{ _block->getTransform()._position });
    }

    void display() override {
        Object::display();
        _block->display();
    }

    void generateBlock() override {
        generateBlockPositions();

        _block = new Polygon(_camera);
        _block->initShader(vertexShader, fragmentShader, /*readFromFile*/ false);
        _block->setDrawMode(GL_TRIANGLES);
        //_block->setTexture("tetris-assets/textures/block-" + _blockIdentifier + ".png");
        _block->initMVP();
        _block->setPositions(_blockPositions);
        _block->init();
    }

    void down() override {
        _block->getTransform()._position.y -= _tetrisProperties.interval;
    }

    void fasterDown() override {}

    void right() override {
        _block->getTransform()._position.x += _tetrisProperties.interval;
    }

    void left() override {
        _block->getTransform()._position.x -= _tetrisProperties.interval;
    }

    void rotateLeft() override {}

    void rotateRight() override {}

private:
    void generatePositions() {
        const auto x = _tetrisProperties.totalCols / 2.0f - 1.0f;
        const auto y = _tetrisProperties.totalRows - 2.0f;

        const auto s1 = createSquarePosition(x - 2.0f, y);
        const auto s2 = createSquarePosition(x - 1.0f, y);
        const auto s3 = createSquarePosition(x + 0.0f, y);
        const auto s4 = createSquarePosition(x + 1.0f, y);

        _blockPositions = {
            s1[Positions::bottom_left],
            s1[Positions::top_left],
            s4[Positions::top_right],
            s4[Positions::bottom_right],
        };
    }

    void generateBlockPositions() {
        const auto x = _tetrisProperties.totalCols / 2.0f - 1.0f;
        const auto y = _tetrisProperties.totalRows - 2.0f;

        const auto s1 = createSquarePosition(x - 2.0f, y);
        const auto s2 = createSquarePosition(x - 1.0f, y);
        const auto s3 = createSquarePosition(x + 0.0f, y);
        const auto s4 = createSquarePosition(x + 1.0f, y);

        _blockPositions.reserve(4 * 4);
        _blockPositions.insert(_blockPositions.end(), s1.begin(), s1.end());
        _blockPositions.insert(_blockPositions.end(), s2.begin(), s2.end());
        _blockPositions.insert(_blockPositions.end(), s3.begin(), s3.end());
        _blockPositions.insert(_blockPositions.end(), s4.begin(), s4.end());
    }

    std::string _blockIdentifier{ "I" };
};
