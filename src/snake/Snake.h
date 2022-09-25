#pragma once

#include "../engine/Point.h"
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

    void init() override {
        _food.init();
        _board.init();

        const int x = _snakeProperties.totalRows / 2;
        const int y = _snakeProperties.totalCols / 2;
        addBody(x, y + 3, SnakeMovement::forward, true);
        addBody(x, y + 2);
        addBody(x, y + 1);
        addBody(x, y);
        addBody(x, y - 1);
    }

    void update() override {
        if (_snakeProperties.timeElapsed >= 0.16f && _snakeProperties.isPlaying) {
            _snakeProperties.timeElapsed = 0.0f;

            const auto oldTail = _snake.back();
            const auto interval = _snakeProperties.interval;

            SnakeMovement oldDir{ SnakeMovement::noop };
            SnakeMovement reqDir{ _snake.front().requestedDirection };
            reqDir = reqDir == SnakeMovement::noop ? SnakeMovement::forward : reqDir;
            for (auto& cell : _snake) {
                oldDir = cell.direction;
                cell.direction = reqDir;
                if (SnakeMovement::forward == cell.direction) {
                    cell.y += 1;
                    cell.position += glm::vec2{ 0.0f, interval };
                    cell.entity->setPositionVec2(cell.position);
                }
                else if (SnakeMovement::backward == cell.direction) {
                    cell.y -= 1;
                    cell.position += glm::vec2{ 0.0f, -interval };
                    cell.entity->setPositionVec2(cell.position);
                }
                else if (SnakeMovement::left == cell.direction) {
                    cell.x -= 1;
                    cell.position += glm::vec2{ -interval, 0.0f };
                    cell.entity->setPositionVec2(cell.position);
                }
                else if (SnakeMovement::right == cell.direction) {
                    cell.x += 1;
                    cell.position += glm::vec2{ interval, 0.0f };
                    cell.entity->setPositionVec2(cell.position);
                }
                reqDir = oldDir;
            }

            const auto& head = _snake.front();

            bool isHead = true;
            for (auto& cell : _snake) {
                if (isHead) {
                    isHead = false;
                    continue;
                }
                if (cell.x == head.x && cell.y == head.y) {
                    cell.entity->setColor(_snakeCollisionColor);
                    head.entity->setColor(_snakeCollisionColor);
                    _snakeProperties.isPlaying = false;
                    LOGGER(info, "game over");
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
        for (const auto& cell : _snake) {
            cell.entity->display();
        }

        _food.display();
        _board.display();
    }

    void addBody(const int x, const int y, SnakeMovement direction = SnakeMovement::forward, bool isHead = false) {
        Point* snakeBody = new Point(_camera);
        snakeBody->initShader("snake-assets/shaders/snake_vs.shader", "snake-assets/shaders/snake_fs.shader");
        snakeBody->initMVP();
        snakeBody->setDrawMode(GL_POINTS);
        snakeBody->setPositionsVec2({ { 0.0f, 0.0f } });
        if (isHead) {
            snakeBody->setColor(_snakeHeadColor);
        }
        else {
            snakeBody->setColor(_snakeBodyColor);
        }
        snakeBody->init();

        const auto interval = _snakeProperties.interval;
        const glm::vec2 position{ x * interval + interval / 2.0f, y * interval + interval / 2.0f };
        snakeBody->setPositionVec2(position);

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
