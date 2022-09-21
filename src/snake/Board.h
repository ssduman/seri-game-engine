#pragma once

#include "../engine/Line.h"
#include "../engine/Entity.h"

#include "Camera.h"

#include <vector>

class Board : public Entity {
public:
    Board(Camera* camera) : Entity(camera) {

    }

    void init() override {
        const int xCount = static_cast<int>(_width / _step);
        const int yCount = static_cast<int>(_height / _step);

        _lines = new Line(_camera);
        _lines->initShader("snake-assets/shaders/snake_vs.shader", "snake-assets/shaders/snake_fs.shader");
        _lines->initMVP();
        _lines->setDrawMode(GL_LINES);
        _lines->reserveTotalDataCountVec2(xCount * yCount * 2);
        _lines->setColor(_lineColor);
        _lines->init();

        for (int x = 0; x < xCount; x++) {
            _lines->addPositionsVec2({ { 0.0f, _step * x }, { _width, _step * x } });
        }

        for (int y = 0; y < yCount; y++) {
            _lines->addPositionsVec2({ { _step * y, 0.0f }, { _step * y, _height } });
        }
    }

    void update() override {

    }

    void render() override {
        _lines->display();
    }

private:
    void createLine(float x1, float y1, float x2, float y2) {
        Line* line = new Line(_camera);
        line->initShader("snake-assets/shaders/snake_vs.shader", "snake-assets/shaders/snake_fs.shader");
        line->initMVP();
        line->setDrawMode(GL_LINE_LOOP);
        line->setPositionsVec2({ { x1, y1 }, { x2, y2 } });
        line->setColor(_lineColor);
        line->init();
    }

    Entity* _lines;
    glm::vec4 _lineColor{ 0.2f, 0.2f, 0.2f, 1.0f };

    float _step{ 50.f };
    float _width{ 800.0f };
    float _height{ 800.0f };

};
