#pragma once

#include "../engine/Cube.h"
#include "../engine/Line.h"
#include "../engine/Util.h"
#include "../engine/Point.h"
#include "../engine/Circle.h"
#include "../engine/Entity.h"
#include "../engine/Logger.h"
#include "../engine/Triangle.h"
#include "../engine/Rectangle.h"

#include "Camera.h"

#include <ctime>
#include <cstdlib>

class Factory {
public:
    ~Factory() = default;

    static Entity* CreateEntity(Camera* camera, EntityType entityType) {
        srand(time(0));

        constexpr auto minColor = glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f };
        constexpr auto maxColor = glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f };

        switch (entityType) {
            case EntityType::POINT:
            {
                constexpr auto numSegments = 40;
                std::vector<glm::vec3> positions;
                std::vector<glm::vec4> colors;
                for (int i = 0; i < numSegments; i++) {
                    float theta = 2.0f * PI * static_cast<float>(i) / static_cast<float>(numSegments);
                    float x = 0.5f * cosf(theta);
                    float y = 0.5f * sinf(theta);
                    positions.emplace_back(x, y, 0.0f);
                    colors.push_back(glm::linearRand(minColor, maxColor));
                }
                Point* point = new Point(camera);
                point->initShader("editor-assets/shaders/entity_vs.shader", "editor-assets/shaders/entity_fs.shader");
                point->initMVP();

                point->setDrawMode(GL_POINTS);
                point->setPositions(positions);
                //point->setColor(glm::linearRand(minColor, maxColor));
                point->setColors(colors);

                point->init();

                LOGGER(info, "point created");

                return point;
            }
            case EntityType::LINE:
            {
                std::vector<glm::vec3> positions{ { -0.5f, -0.5f, 0 }, { 0, 0.5f, 0 }, { 0.5f, -0.5f, 0 } };
                std::vector<glm::vec4> colors{ glm::linearRand(minColor, maxColor), glm::linearRand(minColor, maxColor), glm::linearRand(minColor, maxColor) };
                Line* line = new Line(camera);
                line->initShader("editor-assets/shaders/entity_vs.shader", "editor-assets/shaders/entity_fs.shader");
                line->initMVP();

                line->setDrawMode(GL_LINE_LOOP);
                line->setPositions(positions);
                //line->setColor(glm::linearRand(minColor, maxColor));
                line->setColors(colors);

                line->init();

                LOGGER(info, "line created");

                return line;
            }
            case EntityType::TRIANGLE:
            {
                std::vector<glm::vec3> positions{ { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.5f, 0.0f }, { 0.5f, -0.5f, 0.0f } };
                std::vector<glm::vec4> colors{ glm::linearRand(minColor, maxColor), glm::linearRand(minColor, maxColor), glm::linearRand(minColor, maxColor) };
                Triangle* triangle = new Triangle(camera);
                triangle->initShader("editor-assets/shaders/entity_vs.shader", "editor-assets/shaders/entity_fs.shader");
                triangle->initMVP();

                triangle->setPositions(positions);
                triangle->setColor(glm::linearRand(minColor, maxColor));
                triangle->setColors(colors);
                triangle->setTexture("editor-assets/textures/passage.png");

                triangle->init();

                LOGGER(info, "triangle created");

                return triangle;
            }
            case EntityType::RECTANGLE:
            {
                std::vector<glm::vec3> positions{ { -0.5f, -0.5f, 0 }, { -0.5f, 0.5f, 0 }, { 0.5f, 0.5f, 0 }, { 0.5f, -0.5f, 0 } };
                std::vector<glm::vec4> colors{ glm::linearRand(minColor, maxColor), glm::linearRand(minColor, maxColor), glm::linearRand(minColor, maxColor), glm::linearRand(minColor, maxColor) };
                Rectangle* rectangle = new Rectangle(camera);
                rectangle->initShader("editor-assets/shaders/entity_vs.shader", "editor-assets/shaders/entity_fs.shader");
                rectangle->initMVP();

                rectangle->setPositions(positions);
                //rectangle->setColor(glm::linearRand(minColor, maxColor));
                //rectangle->setColors(colors);
                //rectangle->setTexture("editor-assets/textures/wall1.png");

                rectangle->init();

                LOGGER(info, "rectangle created");

                return rectangle;
            }
            case EntityType::CIRCLE:
            {
                constexpr auto numSegments = 40;
                std::vector<glm::vec3> positions;
                std::vector<glm::vec4> colors;
                std::vector<glm::vec2> texturePositions;
                for (int i = 0; i < numSegments; i++) {
                    float theta = 2.0f * PI * static_cast<float>(i) / static_cast<float>(numSegments);
                    float x = 0.5f * cosf(theta);
                    float y = 0.5f * sinf(theta);
                    positions.emplace_back(x, y, 0.0f);
                    colors.push_back(glm::linearRand(minColor, maxColor));
                    texturePositions.emplace_back(x, y);
                }
                Circle* circle = new Circle(camera);
                circle->initShader("editor-assets/shaders/entity_vs.shader", "editor-assets/shaders/entity_fs.shader");
                circle->initMVP();

                circle->setDrawMode(GL_TRIANGLE_FAN);
                circle->setPositions(positions);
                circle->setColor(glm::linearRand(minColor, maxColor));
                //circle->setColors(colors);
                circle->setTexture("editor-assets/textures/passage.png", texturePositions);

                circle->init();

                LOGGER(info, "circle created");

                return circle;
            }
            case EntityType::CUBE:
            {
                std::vector<glm::vec3> positions{
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
                std::vector<glm::vec2> texturePositions{
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

                Cube* cube = new Cube(camera);
                cube->initShader("editor-assets/shaders/entity_vs.shader", "editor-assets/shaders/entity_fs.shader");
                cube->initMVP();

                cube->setPositions(positions);
                cube->setColor(glm::linearRand(minColor, maxColor));
                cube->setTexture("editor-assets/textures/wall2.png", texturePositions);

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