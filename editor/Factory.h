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

                Point* point = new Point(camera);
                point->initShader("editor-assets/shaders/entity_vs.shader", "editor-assets/shaders/entity_fs.shader");
                point->initMVP();

                point->setEngineDimension(aux::Dimension::three_d);
                point->setDrawMode(aux::DrawMode::points);
                point->setDrawArrayCount(positions.size());

                point->useColors(true);

                auto positionsSize = aux::size(positions);
                auto colorsSize = aux::size(colors);
                point->dataBuffer({ /*size*/ positionsSize + colorsSize });

                point->subdataBuffer({ /*offset*/ 0, /*size*/ positionsSize, /*data*/ positions.data() });
                point->attribute({ /*index*/ 0, /*size*/ 3, /*pointer*/ 0 });
                point->subdataBuffer({ /*offset*/ positionsSize, /*size*/ colorsSize, /*data*/ colors.data() });
                point->attribute({ /*index*/ 1, /*size*/ 4, /*pointer*/ (const void*)positionsSize });

                LOGGER(info, "point created");

                return point;
            }
            case EntityType::line:
            {
                std::vector<glm::vec3> positions{ { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.5f, 0.0f }, { 0.5f, -0.5f, 0.0f } };
                std::vector<glm::vec4> colors{ randomColor(), randomColor(), randomColor() };

                Line* line = new Line(camera);
                line->initShader("editor-assets/shaders/entity_vs.shader", "editor-assets/shaders/entity_fs.shader");
                line->initMVP();

                line->setEngineDimension(aux::Dimension::three_d);
                line->setDrawMode(aux::DrawMode::line_loop);
                line->setDrawArrayCount(positions.size());

                line->useColors(true);

                auto positionsSize = aux::size(positions);
                auto colorsSize = aux::size(colors);
                line->dataBuffer({ /*size*/ positionsSize + colorsSize });

                line->subdataBuffer({ /*offset*/ 0, /*size*/ positionsSize, /*data*/ positions.data() });
                line->attribute({ /*index*/ 0, /*size*/ 3, /*pointer*/ 0 });
                line->subdataBuffer({ /*offset*/ positionsSize, /*size*/ colorsSize, /*data*/ colors.data() });
                line->attribute({ /*index*/ 1, /*size*/ 4, /*pointer*/ (const void*)positionsSize });

                LOGGER(info, "line created");

                return line;
            }
            case EntityType::triangle:
            {
                std::vector<glm::vec3> positions{ { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.5f, 0.0f }, { 0.5f, -0.5f, 0.0f } };
                std::vector<glm::vec4> colors{ randomColor(), randomColor(), randomColor() };

                Triangle* triangle = new Triangle(camera);
                triangle->initShader("editor-assets/shaders/entity_vs.shader", "editor-assets/shaders/entity_fs.shader");
                triangle->initMVP();

                triangle->setEngineDimension(aux::Dimension::three_d);
                triangle->setDrawMode(aux::DrawMode::triangles);
                triangle->setDrawArrayCount(positions.size());

                triangle->useColors(true);

                auto positionsSize = aux::size(positions);
                auto colorsSize = aux::size(colors);
                triangle->dataBuffer({ /*size*/ positionsSize + colorsSize });

                triangle->subdataBuffer({ /*offset*/ 0, /*size*/ positionsSize, /*data*/ positions.data() });
                triangle->attribute({ /*index*/ 0, /*size*/ 3, /*pointer*/ 0 });
                triangle->subdataBuffer({ /*offset*/ positionsSize, /*size*/ colorsSize, /*data*/ colors.data() });
                triangle->attribute({ /*index*/ 1, /*size*/ 4, /*pointer*/ (const void*)positionsSize });

                //triangle->setTexture("editor-assets/textures/passage.png");

                LOGGER(info, "triangle created");

                return triangle;
            }
            case EntityType::rectangle:
            {
                std::vector<glm::vec3> positions{ { -0.5f, -0.5f, 0 }, { -0.5f, 0.5f, 0 }, { 0.5f, 0.5f, 0 }, { 0.5f, -0.5f, 0 } };

                Rectangle* rectangle = new Rectangle(camera);
                rectangle->initShader("editor-assets/shaders/entity_vs.shader", "editor-assets/shaders/entity_fs.shader");
                rectangle->initMVP();

                rectangle->setEngineDimension(aux::Dimension::three_d);
                rectangle->setDrawMode(aux::DrawMode::triangles);
                rectangle->setDrawArrayCount(positions.size());

                auto positionsSize = aux::size(positions);
                rectangle->dataBuffer({ /*size*/ positionsSize });

                const std::vector<GLuint> indices{ 0, 1, 3, 1, 2, 3 };
                rectangle->dataBuffer({ aux::Target::ebo, aux::size(indices), indices.data() });

                rectangle->subdataBuffer({ /*offset*/ 0, /*size*/ positionsSize, /*data*/ positions.data() });
                rectangle->attribute({ /*index*/ 0, /*size*/ 3, /*pointer*/ 0 });

                //rectangle->setTexture("editor-assets/textures/wall1.png");

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

                Circle* circle = new Circle(camera);
                circle->initShader("editor-assets/shaders/entity_vs.shader", "editor-assets/shaders/entity_fs.shader");
                circle->initMVP();

                circle->setEngineDimension(aux::Dimension::three_d);
                circle->setDrawMode(aux::DrawMode::triangle_fan);
                circle->setDrawArrayCount(positions.size());

                auto positionsSize = aux::size(positions);
                auto colorsSize = aux::size(colors);
                circle->dataBuffer({ /*size*/ positionsSize });

                circle->subdataBuffer({ /*offset*/ 0, /*size*/ positionsSize, /*data*/ positions.data() });
                circle->attribute({ /*index*/ 0, /*size*/ 3, /*pointer*/ 0 });

                circle->setColor(randomColor());

                //circle->setTexture("editor-assets/textures/passage.png", texturePositions);

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

                Cube* cube = new Cube(camera);
                cube->initShader("editor-assets/shaders/entity_vs.shader", "editor-assets/shaders/entity_fs.shader");
                cube->initMVP();

                cube->setEngineDimension(aux::Dimension::three_d);
                cube->setDrawMode(aux::DrawMode::triangles);
                cube->setDrawArrayCount(positions.size());

                auto positionsSize = aux::size(positions);
                cube->dataBuffer({ /*size*/ positionsSize });

                cube->subdataBuffer({ /*offset*/ 0, /*size*/ positionsSize, /*data*/ positions.data() });
                cube->attribute({ /*index*/ 0, /*size*/ 3, /*pointer*/ 0 });

                cube->setColor(randomColor());

                //cube->setTexture("editor-assets/textures/wall2.png", texturePositions);

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

                polygon->setEngineDimension(aux::Dimension::three_d);
                polygon->setDrawMode(aux::DrawMode::triangle_fan);
                polygon->setDrawArrayCount(positions.size());

                auto positionsSize = aux::size(positions);
                polygon->dataBuffer({ /*size*/ positionsSize });

                polygon->subdataBuffer({ /*offset*/ 0, /*size*/ positionsSize, /*data*/ positions.data() });
                polygon->attribute({ /*index*/ 0, /*size*/ 3, /*pointer*/ 0 });

                //polygon->setTexture("editor-assets/textures/passage.png");

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
