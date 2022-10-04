#pragma once

#include "../engine/Entity.h"
#include "../engine/Logger.h"
#include "../engine/Rectangle.h"

#include "Camera.h"
#include "TetrisProperties.h"

#include <vector>
#include <random>

enum Positions {
    bottom_left = 0,
    top_left,
    top_right,
    bottom_right,
};

class IBlock : public Entity {
public:
    IBlock(Camera* camera, TetrisProperties& tetrisProperties) : Entity(camera), _tetrisProperties(tetrisProperties) {
        _generator.seed(std::random_device{}());
    }

    ~IBlock() override = default;

    void update(float deltaTime) {
        _tetrisProperties.timeElapsed += deltaTime;
    }

    virtual void generateBlock() = 0;

    std::vector<glm::vec2> createSquarePosition(const float x, const float y) {
        const auto interval = _tetrisProperties.interval;
        const auto d1 = (interval * 0.0f) / 2.0f;
        const auto d2 = (interval * 2.0f) / 2.0f;
        return {
            // 2 triangles will be drawn -> v0-v1-v2, v3-v5-v6
            { x * interval + d1, y * interval + d1 }, // bottom left -> v0
            { x * interval + d1, y * interval + d2 }, // top left -> v1
            { x * interval + d2, y * interval + d2 }, // top right -> v2
            { x * interval + d2, y * interval + d2 }, // top right -> v2
            { x * interval + d2, y * interval + d1 }, // bottom right -> v3
            { x * interval + d1, y * interval + d1 }, // bottom left -> v0
        };
    }

    const glm::ivec2& getBlockPosition() {
        return _blockPosition;
    }

protected:
    TetrisProperties& _tetrisProperties;
    Entity* _block{ nullptr };
    glm::ivec2 _blockPosition{};
    std::vector<glm::vec2> _blockPositions;

    std::default_random_engine _generator;
    std::uniform_int_distribution<int> _distributionCols{ 1, 7 };

};
