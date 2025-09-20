#pragma once

#include "seri/core/Seri.h"
#include "seri/shape/Cube.h"
#include "seri/shape/Line.h"
#include "seri/shape/Point.h"
#include "seri/shape/Circle.h"
#include "seri/shape/Polygon.h"
#include "seri/shape/Triangle.h"
#include "seri/shape/Rectangle.h"

#include <memory>
#include <cstdlib>

class ShapeFactory
{
public:
	~ShapeFactory() = default;

	static std::shared_ptr<seri::Entity> CreateShape(const std::shared_ptr<seri::CameraBase>& camera, seri::EntityType entityType)
	{
#if false

		static auto randomColor = []() -> glm::vec4 { return glm::linearRand(glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f }, glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f }); };

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

					auto& shaderInfo = seri::ShaderLibrary::Get("entity");

					auto point = std::make_shared<seri::Point>(camera);
					point->GetShader()->Init(shaderInfo.vsCode.c_str(), shaderInfo.fsCode.c_str(), /*readFromFile*/ false);
					point->Init();

					point->GetDrawer().reserveDataBufferSize(positionsSize + colorsSize);
					point->GetDrawer().setSubDataBuffer(seri::aux::Index::position, positions, 0);
					point->GetDrawer().setSubDataBuffer(seri::aux::Index::color, colors, positionsSize);

					LOGGER(info, "point created");

					return point;
				}
			case seri::EntityType::line:
				{
					std::vector<glm::vec3> positions{ { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.5f, 0.0f }, { 0.5f, -0.5f, 0.0f } };
					std::vector<glm::vec4> colors{ randomColor(), randomColor(), randomColor() };

					auto positionsSize = seri::aux::size(positions);
					auto colorsSize = seri::aux::size(colors);

					auto& shaderInfo = seri::ShaderLibrary::Get("entity");

					auto line = std::make_shared<seri::Line>(camera);
					line->GetShader()->Init(shaderInfo.vsCode.c_str(), shaderInfo.fsCode.c_str(), /*readFromFile*/ false);
					line->Init();

					line->GetDrawer().reserveDataBufferSize(positionsSize + colorsSize);
					line->GetDrawer().setSubDataBuffer(seri::aux::Index::position, positions, 0);
					line->GetDrawer().setSubDataBuffer(seri::aux::Index::color, colors, positionsSize);

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

					auto& shaderInfo = seri::ShaderLibrary::Get("entity");

					auto triangle = std::make_shared<seri::Triangle>(camera);
					triangle->GetShader()->Init(shaderInfo.vsCode.c_str(), shaderInfo.fsCode.c_str(), /*readFromFile*/ false);
					triangle->GetTexture()->Init(seri::TextureDesc{}, "assets/textures/passage.png");
					triangle->Init();

					triangle->GetDrawer().reserveDataBufferSize(positionsSize + colorsSize + texturePositionsSize);
					triangle->GetDrawer().setSubDataBuffer(seri::aux::Index::position, positions, 0);
					triangle->GetDrawer().setSubDataBuffer(seri::aux::Index::color, colors, positionsSize);
					triangle->GetDrawer().setSubDataBuffer(seri::aux::Index::texture, texturePositions, positionsSize + colorsSize);

					LOGGER(info, "triangle created");

					return triangle;
				}
			case seri::EntityType::rectangle:
				{
					std::vector<glm::vec3> positions{ { -0.5f, -0.5f, 0 }, { -0.5f, 0.5f, 0 }, { 0.5f, 0.5f, 0 }, { 0.5f, -0.5f, 0 } };
					std::vector<glm::vec2> texturePositions{ { 1.0f, 1.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 1.0f } };

					auto positionsSize = seri::aux::size(positions);
					auto texturePositionsSize = seri::aux::size(texturePositions);

					auto& shaderInfo = seri::ShaderLibrary::Get("entity");

					auto rectangle = std::make_shared<seri::Rectangle>(camera);
					rectangle->GetShader()->Init(shaderInfo.vsCode.c_str(), shaderInfo.fsCode.c_str(), /*readFromFile*/ false);
					rectangle->GetTexture()->Init(seri::TextureDesc{}, "assets/textures/wall1.png");
					rectangle->Init();

					const std::vector<GLuint> indices{ 0, 1, 3, 1, 2, 3 };
					rectangle->GetDrawer().setDataBuffer(seri::aux::DataBuffer{ seri::aux::Target::ebo, seri::aux::size(indices), indices.data() });
					rectangle->GetDrawer().setDrawCount(seri::aux::count(indices));
					rectangle->GetDrawer().setDrawType(seri::aux::DrawType::elements);
					rectangle->GetDrawer().reserveDataBufferSize(positionsSize + texturePositionsSize);
					rectangle->GetDrawer().setSubDataBuffer(seri::aux::Index::position, positions, 0);
					rectangle->GetDrawer().setSubDataBuffer(seri::aux::Index::texture, texturePositions, positionsSize);

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

					auto& shaderInfo = seri::ShaderLibrary::Get("entity");

					auto circle = std::make_shared<seri::Circle>(camera);
					circle->GetShader()->Init(shaderInfo.vsCode.c_str(), shaderInfo.fsCode.c_str(), /*readFromFile*/ false);
					circle->GetTexture()->Init(seri::TextureDesc{}, "assets/textures/passage.png");
					circle->Init();

					seri::ShaderLibrary::SetColor(circle->GetShader(), randomColor());

					circle->GetDrawer().reserveDataBufferSize(positionsSize + colorsSize + texturePositionsSize);
					circle->GetDrawer().setSubDataBuffer(seri::aux::Index::position, positions, 0);
					circle->GetDrawer().setSubDataBuffer(seri::aux::Index::color, colors, positionsSize);
					circle->GetDrawer().setSubDataBuffer(seri::aux::Index::texture, texturePositions, positionsSize + colorsSize);

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

					auto& shaderInfo = seri::ShaderLibrary::Get("entity");

					auto cube = std::make_shared<seri::Cube>(camera);
					cube->GetShader()->Init(shaderInfo.vsCode.c_str(), shaderInfo.fsCode.c_str(), /*readFromFile*/ false);
					cube->GetTexture()->Init(seri::TextureDesc{}, "assets/textures/wall2.png");
					cube->Init();

					seri::ShaderLibrary::SetColor(cube->GetShader(), randomColor());

					cube->GetDrawer().reserveDataBufferSize(positionsSize + texturePositionsSize);
					cube->GetDrawer().setSubDataBuffer(seri::aux::Index::position, positions, 0);
					cube->GetDrawer().setSubDataBuffer(seri::aux::Index::texture, texturePositions, positionsSize);

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

					auto& shaderInfo = seri::ShaderLibrary::Get("entity");

					auto polygon = std::make_shared<seri::Polygon>(camera);
					polygon->GetShader()->Init(shaderInfo.vsCode.c_str(), shaderInfo.fsCode.c_str(), /*readFromFile*/ false);
					polygon->Init();

					polygon->GetDrawer().setDataBuffer(seri::aux::Index::position, positions);

					LOGGER(info, "polygon created");

					return polygon;
				}
			default:
				{
					return nullptr;
				}
		}

#endif

		return nullptr;
	}

};
