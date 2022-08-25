#pragma once

#include "Util.h"
#include "Entity.h"
#include "Point.h"
#include "Line.h"
#include "Triangle.h"
#include "Rectangle.h"
#include "Circle.h"
#include "Cube.h"
#include "Camera.h"
#include "Logger.h"

#include <stdlib.h>

class Factory {
public:
    virtual ~Factory() {}

    static Entity* CreateEntity(Camera* camera, EntityType entityType) {
        glm::vec3 minColor = glm::vec3{ 0.0f, 0.0f, 0.0f };
        glm::vec3 maxColor = glm::vec3{ 1.0f, 1.0f, 1.0f };

        switch (entityType) {
            case EntityType::POINT:
            {
                auto cx = 0.0f;
                auto cy = 0.0f;
                auto cr = 0.5f;
                auto num_segments = 40;
                std::vector<glm::vec3> pointCoordinates{};
                std::vector<glm::vec3> pointColors{};
                for (int i = 0; i < num_segments; i++) {
                    float theta = 2.0f * PI * float(i) / float(num_segments);
                    float x = cr * cosf(theta);
                    float y = cr * sinf(theta);
                    pointCoordinates.push_back(glm::vec3{ x + cx, y + cy, 0.0f });
                    pointColors.push_back(glm::linearRand(minColor, maxColor));
                };
                EntityProperties pointProperties{ pointCoordinates, pointColors, GL_POINTS };
                Point* point = new Point(camera, pointProperties);
                point->initShader("shaders/entity_vs.shader", "shaders/entity_fs.shader");
                point->initCamera(camera);
                point->init();

                LOGGER(debug, "point created");

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
                line->initShader("shaders/entity_vs.shader", "shaders/entity_fs.shader");
                if (glm::linearRand(0.0f, 1.0f) >= 0.5f) {
                    line->initTexture("textures/passage.png");
                }
                line->initCamera(camera);
                line->init();

                LOGGER(debug, "line created");

                return line;
            }
            case EntityType::TRIANGLE:
            {
                EntityProperties triangleProperties = {
                    { glm::vec3{ -0.5f, -0.5f, 0 }, glm::vec3{ 0, 0.5f, 0 }, glm::vec3{ 0.5f, -0.5f, 0 } },
                    { glm::linearRand(minColor, maxColor), glm::linearRand(minColor, maxColor), glm::linearRand(minColor, maxColor) },
                };
                Triangle* triangle = new Triangle(camera, triangleProperties);
                triangle->initShader("shaders/entity_vs.shader", "shaders/entity_fs.shader");
                if (glm::linearRand(0.0f, 1.0f) >= 0.5f) {
                    triangle->initTexture("textures/passage.png");
                }
                triangle->initCamera(camera);
                triangle->init();

                LOGGER(debug, "triangle created");

                return triangle;
            }
            case EntityType::RECTANGLE:
            {
                EntityProperties rectangleProperties = {
                    { glm::vec3{ -0.5f, -0.5f, 0 }, glm::vec3{ -0.5f, 0.5f, 0 }, glm::vec3{ 0.5f, 0.5f, 0 }, glm::vec3{ 0.5f, -0.5f, 0 } },
                    { glm::linearRand(minColor, maxColor), glm::linearRand(minColor, maxColor), glm::linearRand(minColor, maxColor), glm::linearRand(minColor, maxColor) },
                };
                Rectangle* rectangle = new Rectangle(camera, rectangleProperties);
                rectangle->initShader("shaders/entity_vs.shader", "shaders/entity_fs.shader");
                if (glm::linearRand(0.0f, 1.0f) >= 0.5f) {
                    rectangle->initTexture("textures/wall1.png");
                }
                rectangle->initCamera(camera);
                rectangle->init();

                LOGGER(debug, "rectangle created");

                return rectangle;
            }
            case EntityType::CIRCLE:
            {
                auto cx = 0.0f;
                auto cy = 0.0f;
                auto cr = 0.5f;
                auto num_segments = 40;
                std::vector<glm::vec3> circleCoordinates{};
                std::vector<glm::vec3> circleColors{};
                for (int i = 0; i < num_segments; i++) {
                    float theta = 2.0f * PI * float(i) / float(num_segments);
                    float x = cr * cosf(theta);
                    float y = cr * sinf(theta);
                    circleCoordinates.push_back(glm::vec3{ x + cx, y + cy, 0.0f });
                    circleColors.push_back(glm::linearRand(minColor, maxColor));
                };
                EntityProperties circleProperties{ circleCoordinates, circleColors, GL_TRIANGLE_FAN };
                Circle* circle = new Circle(camera, circleProperties);
                circle->initShader("shaders/entity_vs.shader", "shaders/entity_fs.shader");
                if (glm::linearRand(0.0f, 1.0f) >= 0.5f) {
                    circle->initTexture("textures/passage.png");
                }
                circle->initCamera(camera);
                circle->init();

                LOGGER(debug, "circle created");

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
                cube->initShader("shaders/entity_vs.shader", "shaders/entity_fs.shader");
                cube->initTexture("textures/wall2.png");
                cube->initCamera(camera);
                cube->init();

                LOGGER(debug, "cube created");

                return cube;
            }
            default:
            {
                return nullptr;
            }
        }
    }

private:

};