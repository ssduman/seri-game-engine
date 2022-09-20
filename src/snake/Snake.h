#pragma once

#include "../engine/Point.h"
#include "../engine/Entity.h"
#include "../engine/Triangle.h"

#include "Camera.h"

#include <vector>

enum class SnakeMovement {
    forward,
    backward,
    left,
    right,
};

class Snake : public Entity {
public:
    Snake(Camera* camera) : Entity(camera) {

    }

    void init() override {

    }

    void update() override {

    }

    void render() override {
        for (const auto point : _points) {
            point->display();
        }
    }

    void createPoint(const float x, const float y) {
        Point* point = new Point(_camera);
        point->initShader("snake-assets/shaders/snake_vs.shader", "snake-assets/shaders/snake_fs.shader");
        point->initMVP();
        point->setDrawMode(GL_POINTS);
        point->setPositionsVec2({ { x, y } });
        point->setColor(_pointColor);
        point->init();

        _points.emplace_back(point);
        _pointPositions.emplace_back(0.0f, 0.0f);
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
        triangle->setColor({ 0.2f, 0.2f, 0.2f, 1.0f });
        triangle->init();

        _points.emplace_back(triangle);
    }

    void handleMovement(float deltaTime, SnakeMovement snakeMovement) {
        const auto movementSpeed = movementStep * deltaTime;

        auto& lastPosition = _pointPositions.back();
        if (SnakeMovement::forward == snakeMovement) {
            lastPosition += glm::vec2{ 0.0f, movementSpeed };
        }
        else if (SnakeMovement::backward == snakeMovement) {
            lastPosition += glm::vec2{ 0.0f, -movementSpeed };
        }
        else if (SnakeMovement::left == snakeMovement) {
            lastPosition += glm::vec2{ -movementSpeed, 0.0f };
        }
        else if (SnakeMovement::right == snakeMovement) {
            lastPosition += glm::vec2{ movementSpeed, 0.0f };
        }
        const auto lastPoint = _points.back();
        lastPoint->setPositionVec2(lastPosition);
    }

private:
    std::vector<Entity*> _points;
    std::vector<glm::vec2> _pointPositions;
    glm::vec4 _pointColor{ 0.1f, 1.0f, 0.4f, 1.0f };

    float movementStep = 800.0f;
};
