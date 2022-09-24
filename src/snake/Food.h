#pragma once

#include "../engine/Entity.h"
#include "../engine/Rectangle.h"

#include "Camera.h"

#include <vector>
#include <random>

class Food : public Entity {
public:
    Food(Camera* camera) : Entity(camera) {
        _generator.seed(std::random_device{}());
    }

    void init() override {}

    void update() override {}

    void render() override {}

    void display() override {
        Object::display();
        _food->display();
    }

    void generateFood() {
        const auto x = _distribution(_generator);
        const auto y = _distribution(_generator);
        _foodPosition = { x, y };

        const auto d1 = (_step * 1.0f) / 3.0f;
        const auto d2 = (_step * 2.0f) / 3.0f;
        _foodPositions = {
            { x * _step + d1, y * _step + d1 },
            { x * _step + d1, y * _step + d2 },
            { x * _step + d2, y * _step + d2 },
            { x * _step + d2, y * _step + d1 },
        };
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
    Entity* _food{ nullptr };
    glm::ivec2 _foodPosition;
    std::vector<glm::vec2> _foodPositions;
    glm::vec4 _foodColor{ 0.6f, 2.0f, 0.4f, 1.0f };

    float _step{ 50.f };
    float _width{ 800.0f };
    float _height{ 800.0f };

    std::default_random_engine _generator;
    std::uniform_int_distribution<int> _distribution{ 0, static_cast<int>(_width / _step) - 1 };

};
