#pragma once

#include "../engine/Line.h"
#include "../engine/Point.h"
#include "../engine/Layer.h"
#include "../engine/Color.h"

#include "Camera.h"

#include <cmath>
#include <chrono>
#include <random>
#include <algorithm>

class PerlinNoise {
public:
    PerlinNoise(Camera* camera, Layer& layers) : _camera(camera), _layers(layers) {
        makePermutation();
    }

    void operator()() {
        perlin();

        constexpr float z = 80.0f;
        constexpr float d = 10.0f;
        constexpr float dx = -25.0f;
        constexpr float dy = -10.0f;

        std::vector<glm::vec3> positions{};
        std::vector<glm::vec4> colors{};

        for (int x = 0; x < _pixels.size(); x++) {
            for (int y = 0; y < _pixels[0].size(); y++) {
                auto color = _pixels[x][y];
                positions.emplace_back(x / d + dx, y / d + dy, z);
                colors.emplace_back(color.r, color.g, 1.0f, 1.0f);
            }
        }

        Point* perlinNoisePoints = new Point(_camera);
        perlinNoisePoints->initShader("mics-assets/shaders/entity_vs.shader", "mics-assets/shaders/entity_fs.shader");
        perlinNoisePoints->initMVP();

        perlinNoisePoints->setDrawMode(GL_POINTS);
        perlinNoisePoints->setPositions(positions);
        perlinNoisePoints->setColors(colors);

        perlinNoisePoints->init();

        _layers.addLayer(perlinNoisePoints);

        LOGGER(info, "perlin noise created with size: " << positions.size());
    }

    std::vector<std::vector<Color>>& getPixels() {
        return _pixels;
    }

private:
    void perlin() {
        for (int y = 0; y < 500; y++) {
            std::vector<Color> row;
            for (int x = 0; x < 500; x++) {
                float n = noise2D(x * 0.01f, y * 0.01f);
                n += 1.0f;
                n /= 2.0f;
                row.emplace_back(n);
            }
            _pixels.emplace_back(row);
        }
    }

    void makePermutation() {
        for (auto i = 0; i < 256; i++) {
            _P.push_back(i);
        }
        shuffle(_P);
        for (auto i = 0; i < 256; i++) {
            _P.push_back(_P[i]);
        }
    }

    void shuffle(std::vector<int>& p) {
        auto rd = std::random_device{};
        auto rng = std::default_random_engine{ rd() };
        std::shuffle(std::begin(p), std::end(p), rng);
    }

    glm::vec2 getConstantVector(int v) {
        const auto dir = v & 3;

        if (dir == 0) {
            return { 1.0, 1.0 };
        }
        if (dir == 1) {
            return { -1.0, 1.0 };
        }
        if (dir == 2) {
            return { -1.0, -1.0 };
        }
        return { 1.0, -1.0 };
    }

    float fade(float t) {
        return t * t * t * (t * (t * 6 - 15) + 10);
    }

    float lerp(float t, float a, float b) {
        return a + t * (b - a);
    }

    float noise2D(float x, float y) {
        int xInt = static_cast<int>(x);
        int yInt = static_cast<int>(y);

        float dx = x - xInt;
        float dy = y - yInt;

        auto topRight = glm::vec2(dx - 1.0, dy - 1.0);
        auto topLeft = glm::vec2(dx, dy - 1.0);
        auto bottomRight = glm::vec2(dx - 1.0, dy);
        auto bottomLeft = glm::vec2(dx, dy);

        auto valueTopRight = _P[_P[xInt + 1] + yInt + 1];
        auto valueTopLeft = _P[_P[xInt] + yInt + 1];
        auto valueBottomRight = _P[_P[xInt + 1] + yInt];
        auto valueBottomLeft = _P[_P[xInt] + yInt];

        auto dotTopRight = glm::dot(topRight, getConstantVector(valueTopRight));
        auto dotTopLeft = glm::dot(topLeft, getConstantVector(valueTopLeft));
        auto dotBottomRight = glm::dot(bottomRight, getConstantVector(valueBottomRight));
        auto dotBottomLeft = glm::dot(bottomLeft, getConstantVector(valueBottomLeft));

        auto u = fade(dx);
        auto v = fade(dy);

        return lerp(u,
            lerp(v, dotBottomLeft, dotTopLeft),
            lerp(v, dotBottomRight, dotTopRight)
        );
    }

    Camera* _camera;
    Layer& _layers;

    std::vector<int> _P;
    std::vector<std::vector<Color>> _pixels;

};
