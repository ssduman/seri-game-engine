#pragma once

#include "Object.h"
#include "Shader.h"
#include "Texture.h"
#include "Material.h"
#include "ShaderManager.h"
#include "AuxiliaryStructs.h"
#include "OpenGLEngineBackend.h"

#include <glm/glm.hpp>

#include <memory>
#include <string>
#include <vector>

class Mesh : public Object {
public:
    Mesh(Shader& shader) : _shader(shader) {}

    Mesh(Mesh&& other) noexcept : _shader(other._shader) {
        _positions = std::move(other._positions);
        _colors = std::move(other._colors);
        _textureCoords = std::move(other._textureCoords);
        _normals = std::move(other._normals);
        _textures = std::move(other._textures);
        _indices = std::move(other._indices);
        _transformation = std::move(other._transformation);
        _shaderManager = std::move(other._shaderManager);
        _engineBackend = std::move(other._engineBackend);
    }

    Mesh(const Mesh& other) = delete;

    Mesh& operator=(Mesh&& other) = delete;

    Mesh& operator=(const Mesh& other) = delete;

    ~Mesh() override = default;

    void init() override {
        auto drawElementCount = aux::count(_indices);
        if (drawElementCount > 0) {
            _engineBackend.setDrawCount(drawElementCount);
            _engineBackend.setDrawType(aux::DrawType::elements);
            _engineBackend.setDataBuffer(aux::DataBuffer{ aux::Target::ebo, aux::size(_indices), aux::data(_indices) });
        }

        auto positionsSize = aux::size(_positions);
        auto colorsSize = aux::size(_colors);
        auto textureCoordsSize = aux::size(_textureCoords);
        auto normalsSize = aux::size(_normals);

        _engineBackend.reserveDataBufferSize(positionsSize + colorsSize + textureCoordsSize + normalsSize);
        if (positionsSize > 0) {
            _engineBackend.setSubDataBuffer(aux::Index::position, _positions, 0);
        }
        if (colorsSize > 0) {
            _engineBackend.setSubDataBuffer(aux::Index::color, _colors, positionsSize);
        }
        if (textureCoordsSize > 0) {
            _engineBackend.setSubDataBuffer(aux::Index::texture, _textureCoords, positionsSize + colorsSize);
        }
        if (normalsSize > 0) {
            _engineBackend.setSubDataBuffer(aux::Index::normal, _normals, positionsSize + colorsSize + textureCoordsSize);
        }
    }

    void update() override {}

    void render() override {
        _shader.use();
        _shader.setMat4("u_model", _transformation);
        for (auto& texture : _textures) {
            texture.bind();
        }
        _engineBackend.draw();
    }

    void setTransformation(glm::mat4 transformation) {
        _transformation = std::move(transformation);
    }

    void addPositions(std::vector<glm::vec3> positions) {
        _positions.insert(_positions.end(), positions.begin(), positions.end());
    }

    void addColors(std::vector<glm::vec4> colors) {
        _colors.insert(_colors.end(), colors.begin(), colors.end());
    }

    void addTextureCoords(std::vector<glm::vec2> textureCoords) {
        _textureCoords.insert(_textureCoords.end(), textureCoords.begin(), textureCoords.end());
    }

    void addNormals(std::vector<glm::vec3> normals) {
        _normals.insert(_normals.end(), normals.begin(), normals.end());
    }

    void addTextures(std::vector<Texture> textures) {
        for (auto& tex : textures) {
            _textures.emplace_back(std::move(tex));
        }
    }

    void addIndices(std::vector<unsigned int> indices) {
        _indices.insert(_indices.end(), indices.begin(), indices.end());
    }

private:
    Shader& _shader;
    ShaderManager _shaderManager{ _shader };
    OpenGLEngineBackend _engineBackend{ _shaderManager };

    std::vector<glm::vec3> _positions;
    std::vector<glm::vec4> _colors;
    std::vector<glm::vec2> _textureCoords;
    std::vector<glm::vec3> _normals;
    std::vector<Texture> _textures;
    std::vector<unsigned int> _indices;
    glm::mat4 _transformation{ 1.0f };

};
