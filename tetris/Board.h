#pragma once

#include "../engine/Entity.h"
#include "../engine/Line.h"
#include "../engine/Logger.h"

#include "Camera.h"
#include "TetrisShaders.h"
#include "TetrisProperties.h"

#include <vector>

class Board : public Entity {
public:
    Board(Camera* camera, TetrisProperties& tetrisProperties) : Entity(camera), _tetrisProperties(tetrisProperties) {
        LOGGER(info, "board init succeeded");
    }

    ~Board() override {
        delete _lines;

        LOGGER(info, "board delete succeeded");
    }

    void init() override {
        _lines = new Line(_camera);
        _lines->initShader(vertexShader, fragmentShader, /*readFromFile*/ false);
        _lines->initMVP();
        _lines->setDrawMode(GL_LINES);
        _lines->reserveTotalDataCountVec2(_tetrisProperties.totalCells * 2);
        _lines->setColor(_lineColor);
        _lines->init();

        const auto width = _tetrisProperties.width;
        const auto height = _tetrisProperties.height;
        const auto interval = _tetrisProperties.interval;
        for (int x = 0; x < _tetrisProperties.totalRows; x++) {
            _lines->addPositions({ glm::vec2{ 0.0f, interval * x }, glm::vec2{ width, interval * x } });
        }
        for (int y = 0; y < _tetrisProperties.totalCols; y++) {
            _lines->addPositions({ glm::vec2{ interval * y, 0.0f }, glm::vec2{ interval * y, height } });
        }
    }

    void update() override {}

    void render() override {}

    void display() override {
        Object::display();
        _lines->display();
    }

private:
    TetrisProperties& _tetrisProperties;
    Entity* _lines{ nullptr };
    glm::vec4 _lineColor{ 0.2f, 0.2f, 0.2f, 1.0f };
};
