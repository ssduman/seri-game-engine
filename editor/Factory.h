#pragma once

#include "../engine/Cube.h"
#include "../engine/Line.h"
#include "../engine/Util.h"
#include "../engine/Point.h"
#include "../engine/Circle.h"
#include "../engine/Entity.h"
#include "../engine/Logger.h"
#include "../engine/Polygon.h"
#include "../engine/Triangle.h"
#include "../engine/Rectangle.h"

#include "Camera.h"

#include <ctime>
#include <cstdlib>

class Factory {
public:
    ~Factory() = default;

    static Entity* CreateEntity(Camera* camera, EntityType entityType) {
        srand(static_cast<unsigned int>(time(0)));

        constexpr auto minColor = glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f };
        constexpr auto maxColor = glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f };
        constexpr auto randomColor = []() -> glm::vec4 {
            return glm::linearRand(minColor, maxColor);
        };

        switch (entityType) {
            case EntityType::point:
            {
                constexpr auto numSegments = 40;
                std::vector<glm::vec3> positions;
                std::vector<glm::vec4> colors;
                for (int i = 0; i < numSegments; i++) {
                    float theta = 2.0f * PI * static_cast<float>(i) / static_cast<float>(numSegments);
                    float x = 0.5f * cosf(theta);
                    float y = 0.5f * sinf(theta);
                    positions.emplace_back(x, y, 0.0f);
                    colors.push_back(randomColor());
                }

                auto positionsSize = aux::size(positions);
                auto colorsSize = aux::size(colors);

                Point* point = new Point(camera);
                point->initShader("editor-assets/shaders/entity_vs.shader", "editor-assets/shaders/entity_fs.shader");
                point->initMVP();

                point->setDrawMode(aux::DrawMode::points);

                point->reserveDataBuffer(positionsSize + colorsSize);
                point->setSubDataBuffer(aux::Index::position, positions, 0);
                point->setSubDataBuffer(aux::Index::color, colors, positionsSize);

                LOGGER(info, "point created");

                return point;
            }
            case EntityType::line:
            {
                std::vector<glm::vec3> positions{ { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.5f, 0.0f }, { 0.5f, -0.5f, 0.0f } };
                std::vector<glm::vec4> colors{ randomColor(), randomColor(), randomColor() };

                auto positionsSize = aux::size(positions);
                auto colorsSize = aux::size(colors);

                Line* line = new Line(camera);
                line->initShader("editor-assets/shaders/entity_vs.shader", "editor-assets/shaders/entity_fs.shader");
                line->initMVP();

                line->setDrawMode(aux::DrawMode::line_loop);

                line->reserveDataBuffer(positionsSize + colorsSize);
                line->setSubDataBuffer(aux::Index::position, positions, 0);
                line->setSubDataBuffer(aux::Index::color, colors, positionsSize);

                LOGGER(info, "line created");

                return line;
            }
            case EntityType::triangle:
            {
                std::vector<glm::vec3> positions{ { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.5f, 0.0f }, { 0.5f, -0.5f, 0.0f } };
                std::vector<glm::vec4> colors{ randomColor(), randomColor(), randomColor() };
                std::vector<glm::vec2> texturePositions{ { 1.0f, 1.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 1.0f } };

                auto positionsSize = aux::size(positions);
                auto colorsSize = aux::size(colors);
                auto texturePositionsSize = aux::size(texturePositions);

                Triangle* triangle = new Triangle(camera);
                triangle->initShader("editor-assets/shaders/entity_vs.shader", "editor-assets/shaders/entity_fs.shader");
                triangle->initTexture("editor-assets/textures/passage.png");
                triangle->initMVP();

                triangle->setDrawMode(aux::DrawMode::triangles);

                triangle->reserveDataBuffer(positionsSize + colorsSize + texturePositionsSize);
                triangle->setSubDataBuffer(aux::Index::position, positions, 0);
                triangle->setSubDataBuffer(aux::Index::color, colors, positionsSize);
                triangle->setSubDataBuffer(aux::Index::texture, texturePositions, positionsSize + colorsSize);

                LOGGER(info, "triangle created");

                return triangle;
            }
            case EntityType::rectangle:
            {
                std::vector<glm::vec3> positions{ { -0.5f, -0.5f, 0 }, { -0.5f, 0.5f, 0 }, { 0.5f, 0.5f, 0 }, { 0.5f, -0.5f, 0 } };
                std::vector<glm::vec2> texturePositions{ { 1.0f, 1.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 1.0f } };

                auto positionsSize = aux::size(positions);
                auto texturePositionsSize = aux::size(texturePositions);

                Rectangle* rectangle = new Rectangle(camera);
                rectangle->initShader("editor-assets/shaders/entity_vs.shader", "editor-assets/shaders/entity_fs.shader");
                rectangle->initTexture("editor-assets/textures/wall1.png");
                rectangle->initMVP();

                rectangle->setDrawMode(aux::DrawMode::triangles);

                const std::vector<GLuint> indices{ 0, 1, 3, 1, 2, 3 };
                rectangle->dataBuffer({ aux::Target::ebo, aux::size(indices), indices.data() });

                rectangle->reserveDataBuffer(positionsSize + texturePositionsSize);
                rectangle->setSubDataBuffer(aux::Index::position, positions, 0);
                rectangle->setSubDataBuffer(aux::Index::texture, texturePositions, positionsSize);

                LOGGER(info, "rectangle created");

                return rectangle;
            }
            case EntityType::circle:
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
                    colors.push_back(randomColor());
                    texturePositions.emplace_back(x, y);
                }

                auto positionsSize = aux::size(positions);
                auto colorsSize = aux::size(colors);
                auto texturePositionsSize = aux::size(texturePositions);

                Circle* circle = new Circle(camera);
                circle->initShader("editor-assets/shaders/entity_vs.shader", "editor-assets/shaders/entity_fs.shader");
                circle->initTexture("editor-assets/textures/passage.png");
                circle->initMVP();

                circle->setColor(randomColor());
                circle->setDrawMode(aux::DrawMode::triangle_fan);

                circle->reserveDataBuffer(positionsSize + colorsSize + texturePositionsSize);
                circle->setSubDataBuffer(aux::Index::position, positions, 0);
                circle->setSubDataBuffer(aux::Index::color, colors, positionsSize);
                circle->setSubDataBuffer(aux::Index::texture, texturePositions, positionsSize + colorsSize);

                LOGGER(info, "circle created");

                return circle;
            }
            case EntityType::cube:
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

                auto positionsSize = aux::size(positions);
                auto texturePositionsSize = aux::size(texturePositions);

                Cube* cube = new Cube(camera);
                cube->initShader("editor-assets/shaders/entity_vs.shader", "editor-assets/shaders/entity_fs.shader");
                cube->initTexture("editor-assets/textures/wall2.png");
                cube->initMVP();

                cube->setColor(randomColor());
                cube->setDrawMode(aux::DrawMode::triangles);

                cube->reserveDataBuffer(positionsSize + texturePositionsSize);
                cube->setSubDataBuffer(aux::Index::position, positions, 0);
                cube->setSubDataBuffer(aux::Index::texture, texturePositions, positionsSize);

                LOGGER(info, "cube created");

                return cube;
            }
            case EntityType::polygon:
            {
                std::vector<glm::vec3> positions{
                    { -0.5f, -0.5f, 0.0f },
                    { -0.5f, 0.5f, 0.0f },
                    { 0.5f, 0.5f, 0.0f },
                    { 0.0f, 0.0f, 0.0f },
                    { 0.5f, -0.5f, 0.0f },
                };

                Polygon* polygon = new Polygon(camera);
                polygon->initShader("editor-assets/shaders/entity_vs.shader", "editor-assets/shaders/entity_fs.shader");
                polygon->initMVP();

                polygon->setDrawMode(aux::DrawMode::triangle_fan);

                polygon->setDataBuffer(aux::Index::position, positions);

                LOGGER(info, "polygon created");

                return polygon;
            }
            case EntityType::unknown:
            {
                return nullptr;
            }
        }

        return nullptr;
    }

};
