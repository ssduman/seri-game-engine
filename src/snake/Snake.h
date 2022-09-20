#pragma once

#include "../engine/Point.h"
#include "../engine/Entity.h"
#include "../engine/Triangle.h"

#include "Camera.h"

#include <vector>

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

        point->setPositionVec2({ -300.0f, 300.0f });

        _points.emplace_back(point);
        _pointPositions.emplace_back(x, y);
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

private:
    std::vector<Entity*> _points;
    std::vector<glm::vec2> _pointPositions;
    glm::vec4 _pointColor{ 0.1f, 1.0f, 0.4f, 1.0f };

};
