#pragma once

#include "Entity.h"
#include "Texture.h"

#include <glm/glm.hpp>

#include <string>
#include <memory>
#include <vector>

class Mesh : public Entity {
public:
    Mesh() = delete;

    Mesh(Mesh&& other) noexcept : Entity(std::move(other)) {
        _positions = other._positions;
        _colors = other._colors;
        _textureCoords = other._textureCoords;
        _normals = other._normals;
        _textures = std::move(other._textures);
        _indices = other._indices;
        _drawElementCount = other._drawElementCount;
    }

    Mesh(const Mesh& other) = delete;

    Mesh& operator=(Mesh&& other) = delete;

    Mesh& operator=(const Mesh& other) = delete;

    Mesh(ICamera* camera) : Entity(camera) {}

    ~Mesh() override = default;

    void init() override {
        _drawElementCount = static_cast<unsigned int>(_indices.size());

        auto positionsSize = aux::size(_positions);
        auto colorsSize = aux::size(_colors);
        auto textureCoordsSize = aux::size(_textureCoords);
        auto normalsSize = aux::size(_normals);

        reserveDataBuffer(positionsSize + colorsSize + textureCoordsSize + normalsSize);
        setSubDataBuffer(aux::Index::position, _positions, 0);
        setSubDataBuffer(aux::Index::color, _colors, positionsSize);
        setSubDataBuffer(aux::Index::texture, _textureCoords, positionsSize + colorsSize);
        setSubDataBuffer(aux::Index::normal, _normals, positionsSize + colorsSize + textureCoordsSize);

        dataBuffer({ aux::Target::ebo, aux::size(_indices), aux::data(_indices) });
    }

    void render() override {
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr = 1;
        unsigned int heightNr = 1;
        for (unsigned int i = 0; i < _textures.size(); i++) {
            std::string number;
            std::string typeName = _textures[i].getTypeName();
            if (typeName == "texture_diffuse") {
                number = std::to_string(diffuseNr++);
            }
            else if (typeName == "texture_specular") {
                number = std::to_string(specularNr++);
            }
            else if (typeName == "texture_normal") {
                number = std::to_string(normalNr++);
            }
            else if (typeName == "texture_height") {
                number = std::to_string(heightNr++);
            }

            glActiveTexture(GL_TEXTURE0 + i);
            std::string nameInShader = "u_texture"; // typeName + number
            _shader.setInt(nameInShader, i);
            _textures[i].bind();
        }

        glBindVertexArray(_VAO);
        glDrawElements(GL_TRIANGLES, _drawElementCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE0);
    }

    void setShader(Shader& shader) {
        _shader = shader;
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
    std::vector<glm::vec3> _positions;
    std::vector<glm::vec4> _colors;
    std::vector<glm::vec2> _textureCoords;
    std::vector<glm::vec3> _normals;
    std::vector<Texture> _textures;
    std::vector<unsigned int> _indices;

    unsigned int _drawElementCount{ 0 };

};
