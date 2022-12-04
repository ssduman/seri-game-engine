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
#include <memory>
#include <cstdlib>

class Factory {
public:
    ~Factory() = default;

    static std::shared_ptr<Entity> CreateEntity(std::shared_ptr<Camera> camera, EntityType entityType) {
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

                auto point = std::make_shared<Point>(camera);
                point->getShader().init("editor-assets/shaders/entity_vs.shader", "editor-assets/shaders/entity_fs.shader");
                point->init();

                point->getDrawer().reserveDataBufferSize(positionsSize + colorsSize);
                point->getDrawer().setSubDataBuffer(aux::Index::position, positions, 0);
                point->getDrawer().setSubDataBuffer(aux::Index::color, colors, positionsSize);

                LOGGER(info, "point created");

                return point;
            }
            case EntityType::line:
            {
                std::vector<glm::vec3> positions{ { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.5f, 0.0f }, { 0.5f, -0.5f, 0.0f } };
                std::vector<glm::vec4> colors{ randomColor(), randomColor(), randomColor() };

                auto positionsSize = aux::size(positions);
                auto colorsSize = aux::size(colors);

                auto line = std::make_shared<Line>(camera);
                line->getShader().init("editor-assets/shaders/entity_vs.shader", "editor-assets/shaders/entity_fs.shader");
                line->init();

                line->getDrawer().reserveDataBufferSize(positionsSize + colorsSize);
                line->getDrawer().setSubDataBuffer(aux::Index::position, positions, 0);
                line->getDrawer().setSubDataBuffer(aux::Index::color, colors, positionsSize);

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

                auto triangle = std::make_shared<Triangle>(camera);
                triangle->getShader().init("editor-assets/shaders/entity_vs.shader", "editor-assets/shaders/entity_fs.shader");
                triangle->getTexture().init("editor-assets/textures/passage.png");
                triangle->init();

                triangle->getDrawer().reserveDataBufferSize(positionsSize + colorsSize + texturePositionsSize);
                triangle->getDrawer().setSubDataBuffer(aux::Index::position, positions, 0);
                triangle->getDrawer().setSubDataBuffer(aux::Index::color, colors, positionsSize);
                triangle->getDrawer().setSubDataBuffer(aux::Index::texture, texturePositions, positionsSize + colorsSize);

                LOGGER(info, "triangle created");

                return triangle;
            }
            case EntityType::rectangle:
            {
                std::vector<glm::vec3> positions{ { -0.5f, -0.5f, 0 }, { -0.5f, 0.5f, 0 }, { 0.5f, 0.5f, 0 }, { 0.5f, -0.5f, 0 } };
                std::vector<glm::vec2> texturePositions{ { 1.0f, 1.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 1.0f } };

                auto positionsSize = aux::size(positions);
                auto texturePositionsSize = aux::size(texturePositions);

                auto rectangle = std::make_shared<Rectangle>(camera);
                rectangle->getShader().init("editor-assets/shaders/entity_vs.shader", "editor-assets/shaders/entity_fs.shader");
                rectangle->getTexture().init("editor-assets/textures/wall1.png");
                rectangle->init();

                const std::vector<GLuint> indices{ 0, 1, 3, 1, 2, 3 };
                rectangle->getDrawer().setDataBuffer(aux::DataBuffer{ aux::Target::ebo, aux::size(indices), indices.data() });
                rectangle->getDrawer().setDrawCount(aux::count(indices));
                rectangle->getDrawer().setDrawType(aux::DrawType::elements);

                rectangle->getDrawer().reserveDataBufferSize(positionsSize + texturePositionsSize);
                rectangle->getDrawer().setSubDataBuffer(aux::Index::position, positions, 0);
                rectangle->getDrawer().setSubDataBuffer(aux::Index::texture, texturePositions, positionsSize);

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

                auto circle = std::make_shared<Circle>(camera);
                circle->getShader().init("editor-assets/shaders/entity_vs.shader", "editor-assets/shaders/entity_fs.shader");
                circle->getTexture().init("editor-assets/textures/passage.png");
                circle->init();

                circle->getShaderManager().setColor(randomColor());

                circle->getDrawer().reserveDataBufferSize(positionsSize + colorsSize + texturePositionsSize);
                circle->getDrawer().setSubDataBuffer(aux::Index::position, positions, 0);
                circle->getDrawer().setSubDataBuffer(aux::Index::color, colors, positionsSize);
                circle->getDrawer().setSubDataBuffer(aux::Index::texture, texturePositions, positionsSize + colorsSize);

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

                auto cube = std::make_shared<Cube>(camera);
                cube->getShader().init("editor-assets/shaders/entity_vs.shader", "editor-assets/shaders/entity_fs.shader");
                cube->getTexture().init("editor-assets/textures/wall2.png");
                cube->init();

                cube->getShaderManager().setColor(randomColor());

                cube->getDrawer().reserveDataBufferSize(positionsSize + texturePositionsSize);
                cube->getDrawer().setSubDataBuffer(aux::Index::position, positions, 0);
                cube->getDrawer().setSubDataBuffer(aux::Index::texture, texturePositions, positionsSize);

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

                auto polygon = std::make_shared<Polygon>(camera);
                polygon->getShader().init("editor-assets/shaders/entity_vs.shader", "editor-assets/shaders/entity_fs.shader");
                polygon->init();

                polygon->getDrawer().setDataBuffer(aux::Index::position, positions);

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
