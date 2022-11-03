#pragma once

#include "../engine/Line.h"
#include "../engine/Entity.h"
#include "../engine/Logger.h"

#include "Camera.h"
#include "SnakeShaders.h"
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
        std::vector<glm::vec2> linePos;
        const auto width = _snakeProperties.width;
        const auto height = _snakeProperties.height;
        const auto interval = _snakeProperties.interval;
        for (int x = 0; x < _snakeProperties.totalRows; x++) {
            linePos.emplace_back(0.0f, interval * x);
            linePos.emplace_back(width, interval * x);
        }
        for (int y = 0; y < _snakeProperties.totalCols; y++) {
            linePos.emplace_back(interval * y, 0.0f);
            linePos.emplace_back(interval * y, height);
        }

        _lines = new Line(_camera);
        _lines->initShader(vertexShader, fragmentShader, /*readFromFile*/ false);
        _lines->initMVP();
        _lines->setColor(_lineColor);
        _lines->setDrawMode(aux::DrawMode::lines);
        _lines->setDataBuffer(aux::Index::position, linePos);
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
