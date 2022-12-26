#pragma once

#include <core/Seri.h>

#include "Camera.h"

#include <cmath>
#include <random>

class Fractal : public Object {
public:
    Fractal(std::shared_ptr<ICamera> camera) : _camera(camera) {}

    ~Fractal() override = default;

    void init() override {
        _shaderManager.initMVP(_camera);
    };

    void update() override {
        if (_camera) {
            _shaderManager.setView(_camera->getView());
        }
    };

    void render() override {};

    void display() override {
        update();
        render();
        _shader.use();
        _engineBackend.draw();
        _shader.disuse();
    }

    void fern() {
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

        auto positionsSize = aux::size(positions);

        _engineBackend.setDrawMode(aux::DrawMode::points);
        _engineBackend.reserveDataBufferSize(positionsSize);
        _engineBackend.setSubDataBuffer(aux::Index::position, positions, 0);

        _shaderManager.setColor({ 0.0f, 0.6f, 0.16f, 1.0f });

        LOGGER(info, "fern created with size: " << positions.size());
    }

    void tree() {
        std::vector<glm::vec3> positions;

        float angle = 30.0f;
        float startAngle = 90.0f;
        float shortenBy = 0.5f;
        float branchLength = 5.0f;
        float minBranchLength = 0.5f;

        positions.emplace_back(0.0f, 0.0f, 50.0f);
        positions.emplace_back(0.0f, 1.0f * branchLength, 50.0f);

        buildTree(positions, branchLength, minBranchLength, shortenBy, startAngle, angle);

        auto positionsSize = aux::size(positions);

        _engineBackend.setDrawMode(aux::DrawMode::line_loop);
        _engineBackend.reserveDataBufferSize(positionsSize);
        _engineBackend.setSubDataBuffer(aux::Index::position, positions, 0);

        LOGGER(info, "tree created with size: " << positions.size());
    }

    Shader& getShader() {
        return _shader;
    }

    ShaderManager& getShaderManager() {
        return _shaderManager;
    }

private:
    glm::vec3 getUnitVector(float angle) {
        static constexpr float pi = 3.14159265f;

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

    std::shared_ptr<ICamera> _camera;
    Shader _shader;
    Transform _transform;
    ShaderManager _shaderManager{ _shader };
    OpenGLEngineBackend _engineBackend{ _shaderManager };

    std::default_random_engine _generator;
    std::uniform_int_distribution<int> _distribution{ 1, 100 };

};
