#pragma once

#include "../engine/Entity.h"
#include "../engine/Logger.h"
#include "../engine/Rectangle.h"

#include "Camera.h"
#include "TetrisProperties.h"

#include <vector>
#include <random>

class Block : public Entity {
public:
    Block(Camera* camera, TetrisProperties& tetrisProperties) : Entity(camera), _tetrisProperties(tetrisProperties) {
        _generator.seed(std::random_device{}());

        generateRandomBlock();

        LOGGER(info, "block init succeeded");
    }

    ~Block() override {
        delete _block;

        LOGGER(info, "block delete succeeded");
    }

    void init() override {}

    void update() override {}

    void render() override {}

    void display() override {
        Object::display();
        _block->display();
    }

    void update(float deltaTime) {
        _timeElapsed += deltaTime;
    }

    void generateRandomBlock() {
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

        _block = new Rectangle(_camera);
        _block->initShader("tetris-assets/shaders/tetris_vs.shader", "tetris-assets/shaders/tetris_fs.shader");
        _block->setTexture("tetris-assets/textures/tetris-block" + std::to_string(_distributionCols(_generator)) + ".png");
        _block->initMVP();
        _block->setPositionsVec2(_blockPositions);
        _block->init();
    }

    const glm::ivec2& getBlockPosition() {
        return _blockPosition;
    }

private:
    TetrisProperties& _tetrisProperties;
    Entity* _block{ nullptr };
    glm::ivec2 _blockPosition{};
    std::vector<glm::vec2> _blockPositions;

    float _timeElapsed{ 0.0f };

    std::default_random_engine _generator;
    std::uniform_int_distribution<int> _distributionCols{ 1, 7 };

};
