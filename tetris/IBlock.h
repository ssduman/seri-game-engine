#pragma once

#include "../engine/Entity.h"
#include "../engine/Logger.h"
#include "../engine/Rectangle.h"

#include "Camera.h"
#include "TetrisProperties.h"

#include <vector>
#include <random>

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
