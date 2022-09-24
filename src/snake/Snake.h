#pragma once

#include "../engine/Point.h"
#include "../engine/Logger.h"
#include "../engine/Entity.h"

#include "Cell.h"
#include "Food.h"
#include "Camera.h"
#include "SnakeMovement.h"

#include <vector>

class Snake : public Entity {
public:
    Snake(Camera* camera, Food& food) : Entity(camera), _food(food) {}

    void init() override {
        const int totalRows = static_cast<int>(_width / _step);
        const int totalCols = static_cast<int>(_height / _step);
        const int totalCells = totalRows * totalCols;
        const int x = totalRows / 2;
        const int y = totalCols / 2;

        addBody(x, y - 1);
        addBody(x, y);
        addBody(x, y + 1);
        addBody(x, y + 2);
        addBody(x, y + 3);
    }

    void update() override {
        if (_timeElapsed >= 0.16f) {
            _timeElapsed = 0.0f;

            SnakeMovement oldDir{ SnakeMovement::noop };
            SnakeMovement reqDir{ _cells.back().requestedDirection };
            reqDir = reqDir == SnakeMovement::noop ? SnakeMovement::forward : reqDir;
            for (auto cell = _cells.rbegin(); cell != _cells.rend(); ++cell) {
                oldDir = cell->direction;
                cell->direction = reqDir;
                if (SnakeMovement::forward == cell->direction) {
                    cell->y += 1;
                    cell->position += glm::vec2{ 0.0f, _step };
                    cell->entity->setPositionVec2(cell->position);
                }
                else if (SnakeMovement::backward == cell->direction) {
                    cell->y -= 1;
                    cell->position += glm::vec2{ 0.0f, -_step };
                    cell->entity->setPositionVec2(cell->position);
                }
                else if (SnakeMovement::left == cell->direction) {
                    cell->x -= 1;
                    cell->position += glm::vec2{ -_step, 0.0f };
                    cell->entity->setPositionVec2(cell->position);
                }
                else if (SnakeMovement::right == cell->direction) {
                    cell->x += 1;
                    cell->position += glm::vec2{ _step, 0.0f };
                    cell->entity->setPositionVec2(cell->position);
                }
                reqDir = oldDir;
            }

            const auto& head = _cells.back();
            const auto& foodPosition = _food.getFoodPosition();
            if (head.x == foodPosition.x && head.y == foodPosition.y) {
                LOGGER(info, "food will be eaten");
            }
        }
    }

    void render() override {}

    void display() override {
        Object::display();
        for (const auto& cell : _cells) {
            cell.entity->display();
        }
    }

    void addBody(const int x, const int y) {
        Point* snakeBody = new Point(_camera);
        snakeBody->initShader("snake-assets/shaders/snake_vs.shader", "snake-assets/shaders/snake_fs.shader");
        snakeBody->initMVP();
        snakeBody->setDrawMode(GL_POINTS);
        snakeBody->setPositionsVec2({ { 0.0f, 0.0f } });
        snakeBody->setColor(_snakeColor);
        snakeBody->init();

        const glm::vec2 position{ x * _step + _step / 2.0f, y * _step + _step / 2.0f };
        snakeBody->setPositionVec2(position);

        _cells.emplace_back(x, y, snakeBody, position, _snakeHeadDirection);
    }

    void handleMovement(float deltaTime, SnakeMovement snakeMovement) {
        _cells.back().requestedDirection = snakeMovement;
    }

    void handleTime(float deltaTime) {
        _timeElapsed += deltaTime;
    }

private:
    Food& _food;
    std::vector<Cell> _cells;
    glm::vec4 _snakeColor{ 0.1f, 1.0f, 0.4f, 1.0f };

    float _timeElapsed{ 0.0f };
    SnakeMovement _snakeHeadDirection{ SnakeMovement::forward };

    float _step{ 50.f };
    float _width{ 800.0f };
    float _height{ 800.0f };

};
