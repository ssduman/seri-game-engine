#pragma once

#include "../engine/Point.h"
#include "../engine/Entity.h"
#include "../engine/Triangle.h"
#include "../engine/Rectangle.h"

#include "Camera.h"

#include <vector>
#include <random>

class Food : public Entity {
public:
    Food(Camera* camera) : Entity(camera) {
        _generator.seed(std::random_device{}());
    }

    void init() override {

    }

    void update() override {

    }

    void render() override {
        _food->display();
    }

    void generateFood() {
        const auto x = _distribution(_generator);
        const auto y = _distribution(_generator);
        _foodPosition = {
            { x * _step, y * _step },
            { x * _step, y * _step + _step },
            { x * _step + _step, y * _step + _step },
            { x * _step + _step, y * _step },
        };
        Rectangle* rectangle = new Rectangle(_camera);
        rectangle->initShader("snake-assets/shaders/snake_vs.shader", "snake-assets/shaders/snake_fs.shader");
        rectangle->initMVP();
        rectangle->setPositionsVec2(_foodPosition);
        rectangle->setColor(_foodColor);
        rectangle->init();

        _food = rectangle;
    }

    void generatePointFood() {
        const auto x = _distribution(_generator);
        const auto y = _distribution(_generator);
        _foodPosition.emplace_back(x * _step - _step / 2.0f, y * _step - _step / 2.0f);

        Point* point = new Point(_camera);
        point->initShader("snake-assets/shaders/snake_vs.shader", "snake-assets/shaders/snake_fs.shader");
        point->initMVP();
        point->setDrawMode(GL_POINTS);
        point->setPositionsVec2(_foodPosition);
        point->setColor(_foodColor);
        point->init();

        _food = point;
    }

    void createTriangle() {
        std::vector<glm::vec2> positions{
            { 200.0f, 200.0f },
            { 600.0f, 200.0f },
            { 400.0f, 600.0f },
        };
        Triangle* triangle = new Triangle(_camera);
        triangle->initShader("snake-assets/shaders/snake_vs.shader", "snake-assets/shaders/snake_fs.shader");
        triangle->initMVP();
        triangle->setPositionsVec2(positions);
        triangle->setColor(_foodColor);
        triangle->init();

        _food = triangle;
    }

private:
    Entity* _food;
    std::vector<glm::vec2> _foodPosition;
    glm::vec4 _foodColor{ 0.6f, 2.0f, 0.4f, 1.0f };

    float _step{ 50.f };
    float _width{ 800.0f };
    float _height{ 800.0f };

    std::default_random_engine _generator;
    std::uniform_int_distribution<int> _distribution{ 0, static_cast<int>(_width / _step) };

};
