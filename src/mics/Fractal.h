#pragma once

#include "../engine/Line.h"
#include "../engine/Point.h"
#include "../engine/Layer.h"

#include "Camera.h"

#include <random>

class Fractal {
public:
    Fractal(Camera* camera, Layer& layers) : _camera(camera), _layers(layers) {}

    void BarnsleyFern() {
        std::vector<glm::vec3> pointCoordinates{ { 0.0f, 0.0f, 0.0f } };
        std::vector<glm::vec3> pointColors{ { 0.0f, 0.6f, 0.16f } };

        for (int i = 0; i < 50000; i++) {
            const auto x = pointCoordinates[i].x;
            const auto y = pointCoordinates[i].y;
            const auto next = _distribution(_generator);

            if (next == 1) {
                pointCoordinates.emplace_back(x, 0.16f * y, 0.0f);
            }
            if (next >= 2 && next <= 86) {
                pointCoordinates.emplace_back(0.85 * x + 0.04 * y, -0.04 * x + 0.85 * y + 1.6, 0.0f);
            }
            if (next >= 87 && next <= 93) {
                pointCoordinates.emplace_back(0.2 * x - 0.26 * y, 0.23 * x + 0.22 * y + 1.6, 0.0f);
            }
            if (next >= 94 && next <= 100) {
                pointCoordinates.emplace_back(-0.15 * x + 0.28 * y, 0.26 * x + 0.24 * y + 0.44, 0.0f);
            }

            pointColors.emplace_back(0.0f, 0.6f, 0.16f);
        }

        EntityProperties pointProperties{ pointCoordinates, pointColors, GL_POINTS };
        Point* point = new Point(_camera, pointProperties);
        point->setUseSingleColor(false);
        point->initShader("assets/shaders/entity_vs.shader", "assets/shaders/entity_fs.shader");
        point->initCamera(_camera);
        point->init();

        _layers.addLayer(point);

        LOGGER(info, "fractal created");
    }

private:
    Camera* _camera;
    Layer& _layers;

    std::default_random_engine _generator;
    std::uniform_int_distribution<int> _distribution{ 1, 100 };

};
