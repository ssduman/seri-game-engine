#pragma once

#include "../engine/Logger.h"
#include "../engine/Entity.h"

#include "Cell.h"
#include "Food.h"
#include "Board.h"
#include "Camera.h"
#include "SnakeMovement.h"
#include "SnakeProperties.h"

#include <vector>

class Snake : public Entity {
public:
    Snake(Camera* camera, SnakeProperties& snakeProperties)
        : Entity(camera), _snakeProperties(snakeProperties), _food(camera, _snakeProperties), _board(camera, _snakeProperties) {
        LOGGER(info, "snake init succeeded");
    }

    ~Snake() override {
        for (auto& snake : _snake) {
            delete snake.entity;
        }

        LOGGER(info, "snake delete succeeded");
    }

    void init() override {
        _food.init();
        _board.init();

        const int x = _snakeProperties.totalRows / 2;
        const int y = _snakeProperties.totalCols / 2;
        addBody(x, y - 0, SnakeMovement::forward, true);
        addBody(x, y - 1, SnakeMovement::forward);
        addBody(x, y - 2, SnakeMovement::forward);
    }

    void update() override {
        if (_snakeProperties.timeElapsed >= (0.16f / _snakeProperties.speed) && _snakeProperties.isPlaying) {
            _snakeProperties.timeElapsed = 0.0f;

            const auto oldTail = _snake.back();
            const auto interval = _snakeProperties.interval;

            SnakeMovement oldDir{ SnakeMovement::noop };
            SnakeMovement reqDir{ _snake.front().requestedDirection };
            reqDir = reqDir == SnakeMovement::noop ? SnakeMovement::forward : reqDir;
            for (auto& snake : _snake) {
                oldDir = snake.direction;
                snake.direction = reqDir;
                if (SnakeMovement::forward == snake.direction) {
                    snake.y += 1;
                    snake.position += glm::vec2{ 0.0f, interval };
                    snake.entity->setPositionVec2(snake.position);
                }
                else if (SnakeMovement::backward == snake.direction) {
                    snake.y -= 1;
                    snake.position += glm::vec2{ 0.0f, -interval };
                    snake.entity->setPositionVec2(snake.position);
                }
                else if (SnakeMovement::left == snake.direction) {
                    snake.x -= 1;
                    snake.position += glm::vec2{ -interval, 0.0f };
                    snake.entity->setPositionVec2(snake.position);
                }
                else if (SnakeMovement::right == snake.direction) {
                    snake.x += 1;
                    snake.position += glm::vec2{ interval, 0.0f };
                    snake.entity->setPositionVec2(snake.position);
                }
                reqDir = oldDir;
            }

            const auto& head = _snake.front();
            if (head.x < 0 || head.x >= _snakeProperties.totalCols || head.y < 0 || head.y >= _snakeProperties.totalRows) {
                for (auto& snake : _snake) {
                    snake.entity->setColor(_snakeCollisionColor);
                }
                _snakeProperties.isPlaying = false;
                LOGGER(info, "snake hit the wall, game over");
                return;
            }

            bool isHead = true;
            for (auto& snake : _snake) {
                if (isHead) {
                    isHead = false;
                    continue;
                }
                if (snake.x == head.x && snake.y == head.y) {
                    snake.entity->setColor(_snakeCollisionColor);
                    head.entity->setColor(_snakeCollisionColor);
                    _snakeProperties.isPlaying = false;
                    LOGGER(info, "snake hit own body, game over");
                    return;
                }
            }

            const auto& foodPosition = _food.getFoodPosition();
            if (head.x == foodPosition.x && head.y == foodPosition.y) {
                addBody(oldTail.x, oldTail.y, oldTail.direction);
                _food.generateFood();
                LOGGER(info, "snake ate the food");
            }
        }
    }

    void render() override {}

    void display() override {
        Object::display();
        for (const auto& snake : _snake) {
            snake.entity->display();
        }

        _food.display();
        _board.display();
    }

    void addBody(const int x, const int y, SnakeMovement direction, bool isHead = false) {
        const auto interval = _snakeProperties.interval;
        const auto d1 = (interval * 0.0f) / 2.0f;
        const auto d2 = (interval * 2.0f) / 2.0f;
        const glm::vec2 position{ 0.0f, 0.0f };
        const std::vector<glm::vec2> bodyPositions{
            { x * interval + d1, y * interval + d1 },
            { x * interval + d1, y * interval + d2 },
            { x * interval + d2, y * interval + d2 },
            { x * interval + d2, y * interval + d1 },
        };
        Rectangle* snakeBody = new Rectangle(_camera);
        snakeBody->initShader("snake-assets/shaders/snake_vs.shader", "snake-assets/shaders/snake_fs.shader");
        snakeBody->initMVP();
        snakeBody->setPositionsVec2(bodyPositions);
        if (isHead) {
            snakeBody->setColor(_snakeHeadColor);
        }
        else {
            snakeBody->setColor(_snakeBodyColor);
        }
        snakeBody->init();

        _snake.emplace_back(x, y, snakeBody, position, direction);
    }

    void handleMovement(float deltaTime, SnakeMovement snakeMovement) {
        _snake.front().requestedDirection = snakeMovement;
    }

    void handleTime(float deltaTime) {
        _snakeProperties.timeElapsed += deltaTime;
    }

private:
    SnakeProperties& _snakeProperties;
    Food _food;
    Board _board;

    std::vector<Cell> _snake;
    glm::vec4 _snakeHeadColor{ 0.8f, 1.0f, 0.4f, 1.0f };
    glm::vec4 _snakeBodyColor{ 0.1f, 1.0f, 0.4f, 1.0f };
    glm::vec4 _snakeCollisionColor{ 0.9f, 0.2f, 0.2f, 1.0f };

};
