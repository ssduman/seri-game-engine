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
        std::vector<glm::vec3> pointCoordinates{ { 0.0f, 0.0f, 0.0f } };
        std::vector<glm::vec3> pointColors{ { 0.0f, 0.6f, 0.16f } };

        constexpr int iteration = 50000;
        for (int i = 0; i < iteration; i++) {
            const auto x = pointCoordinates[i].x;
            const auto y = pointCoordinates[i].y;
            const auto next = _distribution(_generator);

            if (next == 1) {
                pointCoordinates.emplace_back(x, 0.16f * y, 0.0f);
            }
            if (next >= 2 && next <= 86) {
                pointCoordinates.emplace_back(0.85f * x + 0.04f * y, -0.04f * x + 0.85f * y + 1.6f, 0.0f);
            }
            if (next >= 87 && next <= 93) {
                pointCoordinates.emplace_back(0.2f * x - 0.26f * y, 0.23f * x + 0.22f * y + 1.6f, 0.0f);
            }
            if (next >= 94 && next <= 100) {
                pointCoordinates.emplace_back(-0.15f * x + 0.28f * y, 0.26f * x + 0.24f * y + 0.44f, 0.0f);
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

        LOGGER(info, "Barnsley Fern created with size: " << pointCoordinates.size());
    }

    void BarnsleyFernAnimation(float deltaTime) {
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
                    pointCoordinates.emplace_back(0.85f * x + 0.04f * y, -0.04f * x + 0.85f * y + 1.6f, 0.0f);
                }
                if (next >= 87 && next <= 93) {
                    pointCoordinates.emplace_back(0.2f * x - 0.26f * y, 0.23f * x + 0.22f * y + 1.6f, 0.0f);
                }
                if (next >= 94 && next <= 100) {
                    pointCoordinates.emplace_back(-0.15f * x + 0.28f * y, 0.26f * x + 0.24f * y + 0.44f, 0.0f);
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

    void FractalTree() {
        std::vector<glm::vec3> lineCoordinates{};
        std::vector<glm::vec3> lineColors{};

        float angle = 30.0f;
        float startAngle = 90.0f;
        float shortenBy = 0.5f;
        float branchLength = 5.0f;
        float minBranchLength = 0.5f;

        lineCoordinates.emplace_back(0.0f, 0.0f, 50.0f);
        lineCoordinates.emplace_back(0.0f, 1.0f * branchLength, 50.0f);

        lineColors.emplace_back(1.0f, 1.0f, 1.0f);
        lineColors.emplace_back(1.0f, 1.0f, 1.0f);

        buildFractalTree(lineCoordinates, lineColors, branchLength, minBranchLength, shortenBy, startAngle, angle);

        EntityProperties lineProperties{
            { lineCoordinates },
            { lineColors },
            GL_LINE_LOOP
        };
        Line* line = new Line(_camera, lineProperties);
        line->setUseSingleColor(false);
        line->initShader("assets/shaders/entity_vs.shader", "assets/shaders/entity_fs.shader");
        line->initCamera(_camera);
        line->init();

        _layers.addLayer(line);

        LOGGER(info, "fractal tree created with size: " << lineCoordinates.size());
    }

private:
    glm::vec3 getUnitVector(float angle) {
        static float pi = 3.14159265f;

        const float radians = angle * pi / 180.0f;
        return { cos(radians), sin(radians), 0.0f };
    }

    void buildFractalTree(std::vector<glm::vec3>& coords, std::vector<glm::vec3>& colors, float branchLen, float minBranchLen, float shortenBy, float dirAngle, float angle) {
        if (branchLen > minBranchLen) {
            const float branchLengthNew = branchLen - shortenBy;

            auto unitVector = getUnitVector(dirAngle);
            auto lastOrigin = coords.back();

            coords.emplace_back(lastOrigin + unitVector * branchLen);
            colors.emplace_back(1.0f, 1.0f, 1.0f);

            dirAngle = dirAngle + angle;
            if (dirAngle >= 360.0f) {
                dirAngle -= 360.0f;
            }

            buildFractalTree(coords, colors, branchLengthNew, minBranchLen, shortenBy, dirAngle, angle);

            dirAngle = dirAngle - angle * 2;
            if (dirAngle <= 0.0f) {
                dirAngle += 360.0f;
            }

            buildFractalTree(coords, colors, branchLengthNew, minBranchLen, shortenBy, dirAngle, angle);

            dirAngle = dirAngle + angle;
            if (dirAngle >= 360.0f) {
                dirAngle -= 360.0f;
            }

            unitVector = getUnitVector(dirAngle);
            lastOrigin = coords.back();

            coords.emplace_back(lastOrigin - unitVector * branchLen);
            colors.emplace_back(1.0f, 1.0f, 1.0f);
        }
    }

    Camera* _camera;
    Layer& _layers;

    std::default_random_engine _generator;
    std::uniform_int_distribution<int> _distribution{ 1, 100 };

};
