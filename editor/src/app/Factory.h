#pragma once

#include "camera/Camera.h"

#include <seri/core/Seri.h>
#include <seri/shape/Cube.h>
#include <seri/shape/Line.h>
#include <seri/shape/Point.h>
#include <seri/shape/Circle.h>
#include <seri/shape/Polygon.h>
#include <seri/shape/Triangle.h>
#include <seri/shape/Rectangle.h>

#include <glm/gtc/random.hpp>

#include <ctime>
#include <memory>
#include <cstdlib>

class Factory
{
public:
	~Factory() = default;

	static std::shared_ptr<seri::Entity> CreateEntity(std::shared_ptr<Camera> camera, seri::EntityType entityType)
	{
		srand(static_cast<unsigned int>(time(0)));

		static auto minColor = glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f };
		static auto maxColor = glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f };
		static auto randomColor = []() -> glm::vec4
			{
				return glm::linearRand(minColor, maxColor);
			};

		switch (entityType)
		{
		case seri::EntityType::point:
		{
			constexpr auto numSegments = 40;
			std::vector<glm::vec3> positions;
			std::vector<glm::vec4> colors;
			for (int i = 0; i < numSegments; i++)
			{
				float theta = 2.0f * PI * static_cast<float>(i) / static_cast<float>(numSegments);
				float x = 0.5f * cosf(theta);
				float y = 0.5f * sinf(theta);
				positions.emplace_back(x, y, 0.0f);
				colors.push_back(randomColor());
			}

			auto positionsSize = seri::aux::size(positions);
			auto colorsSize = seri::aux::size(colors);

			auto point = std::make_shared<seri::Point>(camera);
			point->getShader().init("assets/shaders/entity_vs.shader", "assets/shaders/entity_fs.shader");
			point->init();

			point->getDrawer().reserveDataBufferSize(positionsSize + colorsSize);
			point->getDrawer().setSubDataBuffer(seri::aux::Index::position, positions, 0);
			point->getDrawer().setSubDataBuffer(seri::aux::Index::color, colors, positionsSize);

			LOGGER(info, "point created");

			return point;
		}
		case seri::EntityType::line:
		{
			std::vector<glm::vec3> positions{ { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.5f, 0.0f }, { 0.5f, -0.5f, 0.0f } };
			std::vector<glm::vec4> colors{ randomColor(), randomColor(), randomColor() };

			auto positionsSize = seri::aux::size(positions);
			auto colorsSize = seri::aux::size(colors);

			auto line = std::make_shared<seri::Line>(camera);
			line->getShader().init("assets/shaders/entity_vs.shader", "assets/shaders/entity_fs.shader");
			line->init();

			line->getDrawer().reserveDataBufferSize(positionsSize + colorsSize);
			line->getDrawer().setSubDataBuffer(seri::aux::Index::position, positions, 0);
			line->getDrawer().setSubDataBuffer(seri::aux::Index::color, colors, positionsSize);

			LOGGER(info, "line created");

			return line;
		}
		case seri::EntityType::triangle:
		{
			std::vector<glm::vec3> positions{ { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.5f, 0.0f }, { 0.5f, -0.5f, 0.0f } };
			std::vector<glm::vec4> colors{ randomColor(), randomColor(), randomColor() };
			std::vector<glm::vec2> texturePositions{ { 1.0f, 1.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 1.0f } };

			auto positionsSize = seri::aux::size(positions);
			auto colorsSize = seri::aux::size(colors);
			auto texturePositionsSize = seri::aux::size(texturePositions);

			auto triangle = std::make_shared<seri::Triangle>(camera);
			triangle->getShader().init("assets/shaders/entity_vs.shader", "assets/shaders/entity_fs.shader");
			triangle->getTexture().Init("assets/textures/passage.png");
			triangle->init();

			triangle->getDrawer().reserveDataBufferSize(positionsSize + colorsSize + texturePositionsSize);
			triangle->getDrawer().setSubDataBuffer(seri::aux::Index::position, positions, 0);
			triangle->getDrawer().setSubDataBuffer(seri::aux::Index::color, colors, positionsSize);
			triangle->getDrawer().setSubDataBuffer(seri::aux::Index::texture, texturePositions, positionsSize + colorsSize);

			LOGGER(info, "triangle created");

			return triangle;
		}
		case seri::EntityType::rectangle:
		{
			std::vector<glm::vec3> positions{ { -0.5f, -0.5f, 0 }, { -0.5f, 0.5f, 0 }, { 0.5f, 0.5f, 0 }, { 0.5f, -0.5f, 0 } };
			std::vector<glm::vec2> texturePositions{ { 1.0f, 1.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 1.0f } };

			auto positionsSize = seri::aux::size(positions);
			auto texturePositionsSize = seri::aux::size(texturePositions);

			auto rectangle = std::make_shared<seri::Rectangle>(camera);
			rectangle->getShader().init("assets/shaders/entity_vs.shader", "assets/shaders/entity_fs.shader");
			rectangle->getTexture().Init("assets/textures/wall1.png");
			rectangle->init();

			const std::vector<GLuint> indices{ 0, 1, 3, 1, 2, 3 };
			rectangle->getDrawer().setDataBuffer(seri::aux::DataBuffer{ seri::aux::Target::ebo, seri::aux::size(indices), indices.data() });
			rectangle->getDrawer().setDrawCount(seri::aux::count(indices));
			rectangle->getDrawer().setDrawType(seri::aux::DrawType::elements);

			rectangle->getDrawer().reserveDataBufferSize(positionsSize + texturePositionsSize);
			rectangle->getDrawer().setSubDataBuffer(seri::aux::Index::position, positions, 0);
			rectangle->getDrawer().setSubDataBuffer(seri::aux::Index::texture, texturePositions, positionsSize);

			LOGGER(info, "rectangle created");

			return rectangle;
		}
		case seri::EntityType::circle:
		{
			constexpr auto numSegments = 40;
			std::vector<glm::vec3> positions;
			std::vector<glm::vec4> colors;
			std::vector<glm::vec2> texturePositions;
			for (int i = 0; i < numSegments; i++)
			{
				float theta = 2.0f * PI * static_cast<float>(i) / static_cast<float>(numSegments);
				float x = 0.5f * cosf(theta);
				float y = 0.5f * sinf(theta);
				positions.emplace_back(x, y, 0.0f);
				colors.push_back(randomColor());
				texturePositions.emplace_back(x, y);
			}

			auto positionsSize = seri::aux::size(positions);
			auto colorsSize = seri::aux::size(colors);
			auto texturePositionsSize = seri::aux::size(texturePositions);

			auto circle = std::make_shared<seri::Circle>(camera);
			circle->getShader().init("assets/shaders/entity_vs.shader", "assets/shaders/entity_fs.shader");
			circle->getTexture().Init("assets/textures/passage.png");
			circle->init();

			seri::ShaderManager::GetInstance().SetColor(circle->getShader(), randomColor());

			circle->getDrawer().reserveDataBufferSize(positionsSize + colorsSize + texturePositionsSize);
			circle->getDrawer().setSubDataBuffer(seri::aux::Index::position, positions, 0);
			circle->getDrawer().setSubDataBuffer(seri::aux::Index::color, colors, positionsSize);
			circle->getDrawer().setSubDataBuffer(seri::aux::Index::texture, texturePositions, positionsSize + colorsSize);

			LOGGER(info, "circle created");

			return circle;
		}
		case seri::EntityType::cube:
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

			auto positionsSize = seri::aux::size(positions);
			auto texturePositionsSize = seri::aux::size(texturePositions);

			auto cube = std::make_shared<seri::Cube>(camera);
			cube->getShader().init("assets/shaders/entity_vs.shader", "assets/shaders/entity_fs.shader");
			cube->getTexture().Init("assets/textures/wall2.png");
			cube->init();

			seri::ShaderManager::GetInstance().SetColor(cube->getShader(), randomColor());

			cube->getDrawer().reserveDataBufferSize(positionsSize + texturePositionsSize);
			cube->getDrawer().setSubDataBuffer(seri::aux::Index::position, positions, 0);
			cube->getDrawer().setSubDataBuffer(seri::aux::Index::texture, texturePositions, positionsSize);

			LOGGER(info, "cube created");

			return cube;
		}
		case seri::EntityType::polygon:
		{
			std::vector<glm::vec3> positions{
				{ -0.5f, -0.5f, 0.0f },
				{ -0.5f, 0.5f, 0.0f },
				{ 0.5f, 0.5f, 0.0f },
				{ 0.0f, 0.0f, 0.0f },
				{ 0.5f, -0.5f, 0.0f },
			};

			auto polygon = std::make_shared<seri::Polygon>(camera);
			polygon->getShader().init("assets/shaders/entity_vs.shader", "assets/shaders/entity_fs.shader");
			polygon->init();

			polygon->getDrawer().setDataBuffer(seri::aux::Index::position, positions);

			LOGGER(info, "polygon created");

			return polygon;
		}
		case seri::EntityType::unknown:
		{
			return nullptr;
		}
		}

		return nullptr;
	}

};
