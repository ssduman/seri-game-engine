#pragma once

#include "seri/core/Object.h"
#include "seri/shader/Shader.h"
#include "seri/shader/ShaderManager.h"
#include "seri/model/Material.h"
#include "seri/texture/Texture.h"
#include "seri/renderer/OpenGLEngineBackend.h"

#include <glm/glm.hpp>

#include <memory>
#include <string>
#include <vector>

class Mesh : public Object
{
public:
	Mesh(Shader shader) : _shader(shader) {}

	Mesh(Mesh& other) = default;

	Mesh(Mesh&& other) = default;

	Mesh& operator=(Mesh& other) = delete;

	Mesh& operator=(Mesh&& other) = default;

	~Mesh() override = default;

	void init() override
	{
		auto drawElementCount = aux::count(_indices);
		if (drawElementCount > 0)
		{
			_engineBackend.setDrawCount(drawElementCount);
			_engineBackend.setDrawType(aux::DrawType::elements);
			_engineBackend.setDataBuffer(aux::DataBuffer{ aux::Target::ebo, aux::size(_indices), aux::data(_indices) });
		}

		auto positionsSize = aux::size(_positions);
		auto colorsSize = aux::size(_colors);
		auto textureCoordsSize = aux::size(_textureCoords);
		auto normalsSize = aux::size(_normals);

		_engineBackend.reserveDataBufferSize(positionsSize + colorsSize + textureCoordsSize + normalsSize);
		if (positionsSize > 0)
		{
			_engineBackend.setSubDataBuffer(aux::Index::position, _positions, 0);
		}
		if (colorsSize > 0)
		{
			_engineBackend.setSubDataBuffer(aux::Index::color, _colors, positionsSize);
		}
		if (textureCoordsSize > 0 && _textures.size() > 0)
		{
			_engineBackend.setSubDataBuffer(aux::Index::texture, _textureCoords, positionsSize + colorsSize);
		}
		if (normalsSize > 0)
		{
			_engineBackend.setSubDataBuffer(aux::Index::normal, _normals, positionsSize + colorsSize + textureCoordsSize);
		}

		clearData();
	}

	void update() override {}

	void render() override
	{
		_shader.use();
		for (auto& texture : _textures)
		{
			texture.bind();
		}
		_engineBackend.draw();
		for (auto& texture : _textures)
		{
			texture.unbind();
		}
		_shader.disuse();
	}

	void setTransformation(glm::mat4 transformation)
	{
		_transformation = std::move(transformation);
	}

	void addPositions(std::vector<glm::vec3> positions)
	{
		_positions.insert(_positions.end(), positions.begin(), positions.end());
	}

	void addColors(std::vector<glm::vec4> colors)
	{
		_colors.insert(_colors.end(), colors.begin(), colors.end());
	}

	void addTextureCoords(std::vector<glm::vec2> textureCoords)
	{
		_textureCoords.insert(_textureCoords.end(), textureCoords.begin(), textureCoords.end());
	}

	void addNormals(std::vector<glm::vec3> normals)
	{
		_normals.insert(_normals.end(), normals.begin(), normals.end());
	}

	void addTextures(std::vector<Texture> textures)
	{
		for (auto& tex : textures)
		{
			_textures.emplace_back(std::move(tex));
		}
	}

	void addIndices(std::vector<unsigned int> indices)
	{
		_indices.insert(_indices.end(), indices.begin(), indices.end());
	}

private:
	void clearData()
	{
		_positions.clear();
		_colors.clear();
		_textureCoords.clear();
		_normals.clear();
		_indices.clear();
	}

	Shader _shader;
	OpenGLEngineBackend _engineBackend{};

	std::vector<Texture> _textures;
	std::vector<glm::vec3> _positions;
	std::vector<glm::vec4> _colors;
	std::vector<glm::vec2> _textureCoords;
	std::vector<glm::vec3> _normals;
	std::vector<unsigned int> _indices;
	glm::mat4 _transformation{ 1.0f };

};
