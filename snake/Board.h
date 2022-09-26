#pragma once

#include "../engine/Line.h"
#include "../engine/Logger.h"
#include "../engine/Entity.h"

#include "Camera.h"
#include "SnakeProperties.h"

#include <vector>

class Board : public Entity {
public:
    Board(Camera* camera, SnakeProperties& snakeProperties) : Entity(camera), _snakeProperties(snakeProperties) {
        LOGGER(info, "board init succeeded");
    }

    ~Board() override {
        delete _lines;

        LOGGER(info, "board delete succeeded");
    }

    void init() override {
        _lines = new Line(_camera);
        _lines->initShader("snake-assets/shaders/snake_vs.shader", "snake-assets/shaders/snake_fs.shader");
        _lines->initMVP();
        _lines->setDrawMode(GL_LINES);
        _lines->reserveTotalDataCountVec2(_snakeProperties.totalCells * 2);
        _lines->setColor(_lineColor);
        _lines->init();

        const auto width = _snakeProperties.width;
        const auto height = _snakeProperties.height;
        const auto interval = _snakeProperties.interval;
        for (int x = 0; x < _snakeProperties.totalRows; x++) {
            _lines->addPositionsVec2({ { 0.0f, interval * x }, { width, interval * x } });
        }
        for (int y = 0; y < _snakeProperties.totalCols; y++) {
            _lines->addPositionsVec2({ { interval * y, 0.0f }, { interval * y, height } });
        }
    }

    void update() override {}

    void render() override {}

    void display() override {
        Object::display();
        _lines->display();
    }

private:
    SnakeProperties& _snakeProperties;
    Entity* _lines{ nullptr };
    glm::vec4 _lineColor{ 0.2f, 0.2f, 0.2f, 1.0f };

};
