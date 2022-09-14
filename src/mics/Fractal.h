#pragma once

#include "../engine/Line.h"
#include "../engine/Point.h"
#include "../engine/Layer.h"

#include "Camera.h"

#include <random>

class Fractal {
public:
    Fractal(Camera* camera, Layer& layers) : _camera(camera), _layers(layers) {}

    void BarnsleyFern(float deltaTime) {
        static int i = 0;
        static float dx = 0.0f;
        constexpr int iteration = 50000;

        dx += deltaTime;
        if (dx >= 0.0016f && i < iteration) {
            std::vector<glm::vec3> pointCoordinates{};
            std::vector<glm::vec3> pointColors{};

            if (i == 0) {
                pointCoordinates.emplace_back(0.0f, 0.0f, 0.0f);
                pointColors.emplace_back(0.0f, 0.6f, 0.16f);
            } else {
                Entity* prev = _layers.getLayers().front();
                const auto& prevPointCoordinates = prev->getVertices();
                const auto x = prevPointCoordinates[0];
                const auto y = prevPointCoordinates[1];
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

            dx = 0.0f;
            i++;

            _layers.addLayer(point);
        }
    }

private:
    Camera* _camera;
    Layer& _layers;

    std::default_random_engine _generator;
    std::uniform_int_distribution<int> _distribution{ 1, 100 };

};
