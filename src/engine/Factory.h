#pragma once

#include "Util.h"
#include "Entity.h"
#include "Point.h"
#include "Triangle.h"
#include "Rectangle.h"
#include "Circle.h"
#include "Cube.h"
#include "Camera.h"
#include "Logger.h"

class Factory {
public:
    ~Factory() = default;

    static Entity* CreateEntity(Camera* camera, EntityType entityType) {
        glm::vec3 minColor = glm::vec3{ 0.0f, 0.0f, 0.0f };
        glm::vec3 maxColor = glm::vec3{ 1.0f, 1.0f, 1.0f };

        switch (entityType) {
            case EntityType::POINT:
            {
                auto num_segments = 40;
                std::vector<glm::vec3> pointCoordinates{};
                std::vector<glm::vec3> pointColors{};
                for (int i = 0; i < num_segments; i++) {
                    float theta = 2.0f * PI * static_cast<float>(i) / static_cast<float>(num_segments);
                    float x = 0.5f * cosf(theta);
                    float y = 0.5f * sinf(theta);
                    pointCoordinates.emplace_back(x, y, 0.0f);
                    pointColors.push_back(glm::linearRand(minColor, maxColor));
                }
                EntityProperties pointProperties{ pointCoordinates, pointColors, GL_POINTS };
                Point* point = new Point(camera, pointProperties);
                point->initShader("assets/shaders/entity_vs.shader", "assets/shaders/entity_fs.shader");
                point->initCamera(camera);
                point->init();

                LOGGER(info, "point created");

                return point;
            }
            case EntityType::LINE:
            {
                EntityProperties lineProperties = {
                    { glm::vec3{ -0.5f, -0.5f, 0 }, glm::vec3{ 0, 0.5f, 0 }, glm::vec3{ 0.5f, -0.5f, 0 } },
                    { glm::linearRand(minColor, maxColor), glm::linearRand(minColor, maxColor), glm::linearRand(minColor, maxColor) },
                    GL_LINE_LOOP // GL_LINES GL_LINE_STRIP GL_LINE_LOOP
                };
                Circle* line = new Circle(camera, lineProperties);
                line->initShader("assets/shaders/entity_vs.shader", "assets/shaders/entity_fs.shader");
                if (glm::linearRand(0.0f, 1.0f) >= 0.5f) {
                    line->initTexture("assets/textures/passage.png");
                }
                line->initCamera(camera);
                line->init();

                LOGGER(info, "line created");

                return line;
            }
            case EntityType::TRIANGLE:
            {
                EntityProperties triangleProperties = {
                    { glm::vec3{ -0.5f, -0.5f, 0 }, glm::vec3{ 0, 0.5f, 0 }, glm::vec3{ 0.5f, -0.5f, 0 } },
                    { glm::linearRand(minColor, maxColor), glm::linearRand(minColor, maxColor), glm::linearRand(minColor, maxColor) },
                };
                Triangle* triangle = new Triangle(camera, triangleProperties);
                triangle->initShader("assets/shaders/entity_vs.shader", "assets/shaders/entity_fs.shader");
                if (glm::linearRand(0.0f, 1.0f) >= 0.5f) {
                    triangle->initTexture("assets/textures/passage.png");
                }
                triangle->initCamera(camera);
                triangle->init();

                LOGGER(info, "triangle created");

                return triangle;
            }
            case EntityType::RECTANGLE:
            {
                EntityProperties rectangleProperties = {
                    { glm::vec3{ -0.5f, -0.5f, 0 }, glm::vec3{ -0.5f, 0.5f, 0 }, glm::vec3{ 0.5f, 0.5f, 0 }, glm::vec3{ 0.5f, -0.5f, 0 } },
                    { glm::linearRand(minColor, maxColor), glm::linearRand(minColor, maxColor), glm::linearRand(minColor, maxColor), glm::linearRand(minColor, maxColor) },
                };
                Rectangle* rectangle = new Rectangle(camera, rectangleProperties);
                rectangle->initShader("assets/shaders/entity_vs.shader", "assets/shaders/entity_fs.shader");
                if (glm::linearRand(0.0f, 1.0f) >= 0.5f) {
                    rectangle->initTexture("assets/textures/wall1.png");
                }
                rectangle->initCamera(camera);
                rectangle->init();

                LOGGER(info, "rectangle created");

                return rectangle;
            }
            case EntityType::CIRCLE:
            {
                auto num_segments = 40;
                std::vector<glm::vec3> circleCoordinates{};
                std::vector<glm::vec3> circleColors{};
                for (int i = 0; i < num_segments; i++) {
                    float theta = 2.0f * PI * static_cast<float>(i) / static_cast<float>(num_segments);
                    float x = 0.5f * cosf(theta);
                    float y = 0.5f * sinf(theta);
                    circleCoordinates.emplace_back(x, y, 0.0f);
                    circleColors.push_back(glm::linearRand(minColor, maxColor));
                }
                EntityProperties circleProperties{ circleCoordinates, circleColors, GL_TRIANGLE_FAN };
                Circle* circle = new Circle(camera, circleProperties);
                circle->initShader("assets/shaders/entity_vs.shader", "assets/shaders/entity_fs.shader");
                if (glm::linearRand(0.0f, 1.0f) >= 0.5f) {
                    circle->initTexture("assets/textures/passage.png");
                }
                circle->initCamera(camera);
                circle->init();

                LOGGER(info, "circle created");

                return circle;
            }
            case EntityType::CUBE:
            {
                std::vector<glm::vec3> cubeCoordinates{
                    { -0.5f, -0.5f, -0.5f, },
                    { 0.5f, -0.5f, -0.5f, },
                    { 0.5f, 0.5f, -0.5f, },
                    { 0.5f, 0.5f, -0.5f, },
                    { -0.5f, 0.5f, -0.5f, },
                    { -0.5f, -0.5f, -0.5f, },

                    { -0.5f, -0.5f, 0.5f, },
                    { 0.5f, -0.5f, 0.5f, },
                    { 0.5f, 0.5f, 0.5f, },
                    { 0.5f, 0.5f, 0.5f, },
                    { -0.5f, 0.5f, 0.5f, },
                    { -0.5f, -0.5f, 0.5f, },

                    { -0.5f, 0.5f, 0.5f, },
                    { -0.5f, 0.5f, -0.5f, },
                    { -0.5f, -0.5f, -0.5f, },
                    { -0.5f, -0.5f, -0.5f, },
                    { -0.5f, -0.5f, 0.5f, },
                    { -0.5f, 0.5f, 0.5f, },

                    { 0.5f, 0.5f, 0.5f, },
                    { 0.5f, 0.5f, -0.5f, },
                    { 0.5f, -0.5f, -0.5f, },
                    { 0.5f, -0.5f, -0.5f, },
                    { 0.5f, -0.5f, 0.5f, },
                    { 0.5f, 0.5f, 0.5f, },

                    { -0.5f, -0.5f, -0.5f, },
                    { 0.5f, -0.5f, -0.5f, },
                    { 0.5f, -0.5f, 0.5f, },
                    { 0.5f, -0.5f, 0.5f, },
                    { -0.5f, -0.5f, 0.5f, },
                    { -0.5f, -0.5f, -0.5f, },

                    { -0.5f, 0.5f, -0.5f, },
                    { 0.5f, 0.5f, -0.5f, },
                    { 0.5f, 0.5f, 0.5f, },
                    { 0.5f, 0.5f, 0.5f, },
                    { -0.5f, 0.5f, 0.5f, },
                    { -0.5f, 0.5f, -0.5f, },
                };
                std::vector<glm::vec2> textureCoordinates{
                    { 0.0f, 0.0f },
                    { 1.0f, 0.0f },
                    { 1.0f, 1.0f },
                    { 1.0f, 1.0f },
                    { 0.0f, 1.0f },
                    { 0.0f, 0.0f },

                    { 0.0f, 0.0f },
                    { 1.0f, 0.0f },
                    { 1.0f, 1.0f },
                    { 1.0f, 1.0f },
                    { 0.0f, 1.0f },
                    { 0.0f, 0.0f },

                    { 1.0f, 0.0f },
                    { 1.0f, 1.0f },
                    { 0.0f, 1.0f },
                    { 0.0f, 1.0f },
                    { 0.0f, 0.0f },
                    { 1.0f, 0.0f },

                    { 1.0f, 0.0f },
                    { 1.0f, 1.0f },
                    { 0.0f, 1.0f },
                    { 0.0f, 1.0f },
                    { 0.0f, 0.0f },
                    { 1.0f, 0.0f },

                    { 0.0f, 1.0f },
                    { 1.0f, 1.0f },
                    { 1.0f, 0.0f },
                    { 1.0f, 0.0f },
                    { 0.0f, 0.0f },
                    { 0.0f, 1.0f },

                    { 0.0f, 1.0f },
                    { 1.0f, 1.0f },
                    { 1.0f, 0.0f },
                    { 1.0f, 0.0f },
                    { 0.0f, 0.0f },
                    { 0.0f, 1.0f },
                };

                EntityProperties cubeProperties{ cubeCoordinates, cubeCoordinates, GL_TRIANGLES, textureCoordinates };
                Cube* cube = new Cube(camera, cubeProperties);
                cube->initShader("assets/shaders/entity_vs.shader", "assets/shaders/entity_fs.shader");
                cube->initTexture("assets/textures/wall2.png");
                cube->initCamera(camera);
                cube->init();

                LOGGER(info, "cube created");

                return cube;
            }
            case EntityType::UNKNOWN:
            {
                return nullptr;
            }
            default:
            {
                return nullptr;
            }
        }
    }

};