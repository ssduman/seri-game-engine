#pragma once

#include "../engine/Line.h"
#include "../engine/Point.h"
#include "../engine/Layer.h"

#include "Camera.h"

#include <cmath>
#include <random>

class Fractal {
public:
    Fractal(Camera* camera, Layer& layers) : _camera(camera), _layers(layers) {}

    void BarnsleyFern() {
        constexpr float z = 10.0f;
        constexpr float dy = 2.0f;

        std::vector<glm::vec3> positions{ { 0.0f, dy, z } };

        constexpr int iteration = 50000;
        for (int i = 0; i < iteration; i++) {
            const auto x = positions[i].x;
            const auto y = positions[i].y + dy;

            const auto next = _distribution(_generator);

            if (next == 1) {
                positions.emplace_back(x, 0.16f * y, z);
            }
            if (next >= 2 && next <= 86) {
                positions.emplace_back(0.85f * x + 0.04f * y, -0.04f * x + 0.85f * y + 1.6f, z);
            }
            if (next >= 87 && next <= 93) {
                positions.emplace_back(0.2f * x - 0.26f * y, 0.23f * x + 0.22f * y + 1.6f, z);
            }
            if (next >= 94 && next <= 100) {
                positions.emplace_back(-0.15f * x + 0.28f * y, 0.26f * x + 0.24f * y + 0.44f, z);
            }
        }

        Point* BarnsleyFernPoints = new Point(_camera);
        BarnsleyFernPoints->initShader("mics-assets/shaders/entity_vs.shader", "mics-assets/shaders/entity_fs.shader");
        BarnsleyFernPoints->initMVP();

        BarnsleyFernPoints->setDrawMode(GL_POINTS);
        BarnsleyFernPoints->setPositions(positions);
        BarnsleyFernPoints->setColor({ 0.0f, 0.6f, 0.16f, 1.0f });

        BarnsleyFernPoints->init();

        _layers.addLayer(BarnsleyFernPoints);

        LOGGER(info, "Barnsley Fern created with size: " << positions.size());
    }

    /*
    void BarnsleyFernAnimation(float deltaTime) {
        static int i = 0;
        static float dx = 0.0f;
        constexpr float z = 10.0f;
        constexpr float dy = 2.0f;
        constexpr int iteration = 50000;

        dx += deltaTime;
        if (dx >= 0.0016f && i < iteration) {
            std::vector<glm::vec3> pointCoordinates{};
            std::vector<glm::vec3> pointColors{};

            if (i == 0) {
                pointCoordinates.emplace_back(0.0f, dy, z);
                pointColors.emplace_back(0.0f, 0.6f, 0.16f);
            } else {
                Entity* prev = _layers.getLayers().front();
                const auto& prevPointCoordinates = prev->getVertices();
                const auto x = prevPointCoordinates[0];
                const auto y = prevPointCoordinates[1] + dy;
                const auto next = _distribution(_generator);

                if (next == 1) {
                    pointCoordinates.emplace_back(x, 0.16f * y, z);
                }
                if (next >= 2 && next <= 86) {
                    pointCoordinates.emplace_back(0.85f * x + 0.04f * y, -0.04f * x + 0.85f * y + 1.6f, z);
                }
                if (next >= 87 && next <= 93) {
                    pointCoordinates.emplace_back(0.2f * x - 0.26f * y, 0.23f * x + 0.22f * y + 1.6f, z);
                }
                if (next >= 94 && next <= 100) {
                    pointCoordinates.emplace_back(-0.15f * x + 0.28f * y, 0.26f * x + 0.24f * y + 0.44f, z);
                }

                pointColors.emplace_back(0.0f, 0.6f, 0.16f);
            }

            EntityProperties pointProperties{ pointCoordinates, pointColors, GL_POINTS };
            Point* point = new Point(_camera, pointProperties);
            point->setUseSingleColor(false);
            point->initShader("mics-assets/shaders/entity_vs.shader", "mics-assets/shaders/entity_fs.shader");
            point->initCamera(_camera);
            point->init();

            dx = 0.0f;
            i++;

            _layers.addLayer(point);
        }
    }
    */

    void tree() {
        std::vector<glm::vec3> positions{};

        float angle = 30.0f;
        float startAngle = 90.0f;
        float shortenBy = 0.5f;
        float branchLength = 5.0f;
        float minBranchLength = 0.5f;

        positions.emplace_back(0.0f, 0.0f, 50.0f);
        positions.emplace_back(0.0f, 1.0f * branchLength, 50.0f);

        buildTree(positions, branchLength, minBranchLength, shortenBy, startAngle, angle);

        Line* fractalTreeLines = new Line(_camera);
        fractalTreeLines->initShader("mics-assets/shaders/entity_vs.shader", "mics-assets/shaders/entity_fs.shader");
        fractalTreeLines->initMVP();

        fractalTreeLines->setDrawMode(GL_LINE_LOOP);
        fractalTreeLines->setPositions(positions);

        fractalTreeLines->init();

        _layers.addLayer(fractalTreeLines);

        LOGGER(info, "fractal tree created with size: " << positions.size());
    }

private:
    glm::vec3 getUnitVector(float angle) {
        static float pi = 3.14159265f;

        const float radians = angle * pi / 180.0f;
        return { cos(radians), sin(radians), 0.0f };
    }

    void buildTree(std::vector<glm::vec3>& positions, float branchLen, float minBranchLen, float shortenBy, float dirAngle, float angle) {
        if (branchLen > minBranchLen) {
            const float branchLengthNew = branchLen - shortenBy;

            auto unitVector = getUnitVector(dirAngle);
            auto lastOrigin = positions.back();

            positions.emplace_back(lastOrigin + unitVector * branchLen);

            dirAngle = dirAngle + angle;
            if (dirAngle >= 360.0f) {
                dirAngle -= 360.0f;
            }

            buildTree(positions, branchLengthNew, minBranchLen, shortenBy, dirAngle, angle);

            dirAngle = dirAngle - angle * 2;
            if (dirAngle <= 0.0f) {
                dirAngle += 360.0f;
            }

            buildTree(positions, branchLengthNew, minBranchLen, shortenBy, dirAngle, angle);

            dirAngle = dirAngle + angle;
            if (dirAngle >= 360.0f) {
                dirAngle -= 360.0f;
            }

            unitVector = getUnitVector(dirAngle);
            lastOrigin = positions.back();

            positions.emplace_back(lastOrigin - unitVector * branchLen);
        }
    }

    Camera* _camera;
    Layer& _layers;

    std::default_random_engine _generator;
    std::uniform_int_distribution<int> _distribution{ 1, 100 };

};
