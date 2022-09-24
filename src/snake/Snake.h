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
    Snake(Camera* camera) : Entity(camera) {}

    void init() override {
        const int totalCells = static_cast<int>((_width / _step) * (_height / _step));

        Point* snakeHead = new Point(_camera);
        snakeHead->initShader("snake-assets/shaders/snake_vs.shader", "snake-assets/shaders/snake_fs.shader");
        snakeHead->initMVP();
        snakeHead->setDrawMode(GL_POINTS);
        snakeHead->setPositionsVec2({ { _width / 2.0f - _step / 2.0f, _height / 2.0f - _step / 2.0f } });
        snakeHead->setColor(_pointColor);
        snakeHead->init();

        _snakeBodies.emplace_back(snakeHead);
        _snakeBodiesMovePositions.emplace_back(0.0f, 0.0f);
    }

    void update() override {
        if (_timeElapsed >= 0.16f) {
            _timeElapsed = 0.0f;

            auto& lastPosition = _snakeBodiesMovePositions.back();
            if (SnakeMovement::forward == _snakeHeadDirection) {
                lastPosition += glm::vec2{ 0.0f, _step };
            }
            else if (SnakeMovement::backward == _snakeHeadDirection) {
                lastPosition += glm::vec2{ 0.0f, -_step };
            }
            else if (SnakeMovement::left == _snakeHeadDirection) {
                lastPosition += glm::vec2{ -_step, 0.0f };
            }
            else if (SnakeMovement::right == _snakeHeadDirection) {
                lastPosition += glm::vec2{ _step, 0.0f };
            }
            const auto lastPoint = _snakeBodies.back();
            lastPoint->setPositionVec2(lastPosition);
        }
    }

    void render() override {
        for (const auto& snakeBody : _snakeBodies) {
            snakeBody->display();
        }
    }

    void addBodyToSnakeBodies(const float x, const float y) {
        Point* snakeBody = new Point(_camera);
        snakeBody->initShader("snake-assets/shaders/snake_vs.shader", "snake-assets/shaders/snake_fs.shader");
        snakeBody->initMVP();
        snakeBody->setDrawMode(GL_POINTS);
        snakeBody->setPositionsVec2({ { x, y } });
        snakeBody->setColor(_pointColor);
        snakeBody->init();

        _snakeBodies.emplace_back(snakeBody);
        _snakeBodiesMovePositions.emplace_back(0.0f, 0.0f);
    }

    void handleMovement(float deltaTime, SnakeMovement snakeMovement) {
        _snakeHeadDirection = snakeMovement;
    }

    void handleTime(float deltaTime) {
        _timeElapsed += deltaTime;
    }

private:
    std::vector<Entity*> _snakeBodies;
    std::vector<glm::vec2> _snakeBodiesMovePositions;
    glm::vec4 _pointColor{ 0.1f, 1.0f, 0.4f, 1.0f };

    float _timeElapsed{ 0.0f };
    SnakeMovement _snakeHeadDirection{ SnakeMovement::forward };

    float _step{ 50.f };
    float _width{ 800.0f };
    float _height{ 800.0f };
    float _movementStep = 800.0f;

};
