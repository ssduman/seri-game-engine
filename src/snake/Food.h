#pragma once

#include "../engine/Entity.h"
#include "../engine/Logger.h"
#include "../engine/Rectangle.h"

#include "Camera.h"
#include "SnakeProperties.h"

#include <vector>
#include <random>

class Food : public Entity {
public:
    Food(Camera* camera, SnakeProperties& snakeProperties) : Entity(camera), _snakeProperties(snakeProperties) {
        _generator.seed(std::random_device{}());

        LOGGER(info, "food init succeeded");
    }

    void init() override {
        generateFood();
    }

    void update() override {}

    void render() override {}

    void display() override {
        Object::display();
        _food->display();
    }

    void generateFood() {
        const auto x = _distributionRows(_generator);
        const auto y = _distributionCols(_generator);
        _foodPosition = { x, y };

        const auto interval = _snakeProperties.interval;
        const auto d1 = (interval * 0.0f) / 2.0f;
        const auto d2 = (interval * 2.0f) / 2.0f;
        _foodPositions = {
            { x * interval + d1, y * interval + d1 },
            { x * interval + d1, y * interval + d2 },
            { x * interval + d2, y * interval + d2 },
            { x * interval + d2, y * interval + d1 },
        };

        delete _food;
        _food = new Rectangle(_camera);
        _food->initShader("snake-assets/shaders/snake_vs.shader", "snake-assets/shaders/snake_fs.shader");
        _food->initMVP();
        _food->setPositionsVec2(_foodPositions);
        _food->setColor(_foodColor);
        _food->init();
    }

    const glm::ivec2& getFoodPosition() {
        return _foodPosition;
    }

private:
    SnakeProperties& _snakeProperties;
    Entity* _food{ nullptr };
    glm::ivec2 _foodPosition{};
    std::vector<glm::vec2> _foodPositions;
    glm::vec4 _foodColor{ 0.6f, 2.0f, 0.4f, 1.0f };

    std::default_random_engine _generator;
    std::uniform_int_distribution<int> _distributionRows{ 0, _snakeProperties.totalRows - 1 };
    std::uniform_int_distribution<int> _distributionCols{ 0, _snakeProperties.totalCols - 1 };

};
