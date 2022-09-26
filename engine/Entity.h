#pragma once

#include "Object.h"
#include "Color.h"
#include "Logger.h"
#include "Shader.h"
#include "ICamera.h"
#include "Texture.h"
#include "Transform.h"
#include "EntityType.h"

#include <string>
#include <vector>

class Entity : public Object {
public:
    Entity(ICamera* camera) : _camera(camera) {}

    ~Entity() override = default;

    virtual void initShader(const std::string& vsCodePath, const std::string& fsCodePath) {
        _shader.init(vsCodePath, fsCodePath);
    }

    virtual void initMVP() {
        _shader.use();
        _shader.setMat4("u_model", glm::mat4{ 1.0f });
        _shader.setMat4("u_view", _camera->getView());
        _shader.setMat4("u_projection", _camera->getProjection());
        _shader.disuse();
    }

    virtual void setDrawMode(int drawMode) {
        _drawMode = drawMode;
    }

    virtual void setPositionVec2(const glm::vec2& position) {
        _shader.use();
        _shader.setVec2("u_position", position);
        _shader.disuse();
    }

    virtual void setPosition(const glm::vec3& position) {
        _shader.use();
        _shader.setVec3("u_position", position);
        _shader.disuse();
    }

    virtual void setPositionVec4(const glm::vec4& position) {
        _shader.use();
        _shader.setVec4("u_position", position);
        _shader.disuse();
    }

    virtual void setPositionsVec2(const std::vector<glm::vec2>& positionsDataVec2) {
        _usePositionsVec2 = true;
        _positionsDataVec2 = positionsDataVec2;
        _positionsDataDimension = 2;
        _positionsDataCount = static_cast<int>(_positionsDataVec2.size()) * _positionsDataDimension;
        _totalDataCount += _positionsDataCount;
    }

    virtual void setPositions(const std::vector<glm::vec3>& positionsData) {
        _usePositions = true;
        _positionsData = positionsData;
        _positionsDataDimension = 3;
        _positionsDataCount = static_cast<int>(_positionsData.size()) * _positionsDataDimension;
        _totalDataCount += _positionsDataCount;
    }

    virtual void setPositionsVec4(const std::vector<glm::vec4>& positionsDataVec4) {
        _usePositionsVec4 = true;
        _positionsDataVec4 = positionsDataVec4;
        _positionsDataDimension = 4;
        _positionsDataCount = static_cast<int>(_positionsDataVec4.size()) * _positionsDataDimension;
        _totalDataCount += _positionsDataCount;
    }

    virtual void setColor(const glm::vec4& color) {
        _color = { color };
        _shader.use();
        _shader.setBool("u_useColor", true);
        _shader.setVec4("u_color", _color.getColorRGBA());
        _shader.disuse();
    }

    virtual void setColors(const std::vector<glm::vec4>& colorsData) {
        _useColors = true;
        _shader.use();
        _shader.setBool("u_useColors", _useColors);
        _shader.disuse();

        _colorsData = colorsData;
        _colorsDataDimension = 4;
        _colorsDataCount += static_cast<int>(_colorsData.size()) * _colorsDataDimension;
        _totalDataCount += _colorsDataCount;
    }

    virtual void setTexture(const std::string& texturePath, std::vector<glm::vec2> texturePositionsData = {}) {
        _useTexture = true;
        _shader.use();
        _shader.setBool("u_useTexture", _useTexture);
        _shader.disuse();
        _texture.init(texturePath);

        setTexturePositions(texturePositionsData);
    }

    virtual void setNormals(const std::vector<glm::vec3>& normalsData) {
        _useNormals = true;
        _normalsData = normalsData;
        _normalsDataDimension = 3;
        _normalsDataCount += static_cast<int>(_normalsData.size()) * _normalsDataDimension;
        _totalDataCount += _normalsDataCount;
    }

    virtual void reserveTotalDataCountVec2(int totalDataCount) {
        _totalDataCount += totalDataCount;
        _usePositionsVec2 = true;
        _positionsDataVec2.reserve(_totalDataCount);
        _positionsDataDimension = 2;
    }

    virtual void reserveTotalDataCount(int totalDataCount) {
        _totalDataCount += totalDataCount;
        _usePositions = true;
        _positionsData.reserve(_totalDataCount);
        _positionsDataDimension = 3;
    }

    virtual void reserveTotalDataCountVec4(int totalDataCount) {
        _totalDataCount += totalDataCount;
        _usePositionsVec4 = true;
        _positionsDataVec4.reserve(_totalDataCount);
        _positionsDataDimension = 4;
    }

    virtual void addPositionsVec2(const std::vector<glm::vec2>& positionsData) {
        GLintptr offsetSize = static_cast<int>(_positionsDataVec2.size()) * _positionsDataDimension * sizeof(float);
        _positionsDataVec2.insert(_positionsDataVec2.end(), positionsData.begin(), positionsData.end());
        _positionsDataCount = static_cast<int>(_positionsDataVec2.size()) * _positionsDataDimension;

        // bind vao
        glBindVertexArray(_VAO);
        // bind vbo
        glBindBuffer(GL_ARRAY_BUFFER, _VBO);
        // calculate position data size
        const auto positionsDataSize = _positionsDataCount * sizeof(GLfloat);
        // set position sub data
        glBufferSubData(GL_ARRAY_BUFFER, offsetSize, positionsDataSize, _positionsDataVec2.data());
        // configure position attribute
        glVertexAttribPointer(0, _positionsDataDimension, GL_FLOAT, GL_FALSE, _positionsDataDimension * sizeof(GLfloat), (void*)offsetSize);
        // location defined in shader
        glEnableVertexAttribArray(0);
    }

    virtual void addPositions(const std::vector<glm::vec3>& positionsData) {
        GLintptr offsetSize = static_cast<int>(_positionsData.size()) * _positionsDataDimension * sizeof(float);
        _positionsData.insert(_positionsData.end(), positionsData.begin(), positionsData.end());
        _positionsDataCount = static_cast<int>(_positionsData.size()) * _positionsDataDimension;

        // bind vao
        glBindVertexArray(_VAO);
        // bind vbo
        glBindBuffer(GL_ARRAY_BUFFER, _VBO);
        // calculate position data size
        const auto positionsDataSize = _positionsDataCount * sizeof(GLfloat);
        // set position sub data
        glBufferSubData(GL_ARRAY_BUFFER, offsetSize, positionsDataSize, _positionsData.data());
        // configure position attribute
        glVertexAttribPointer(0, _positionsDataDimension, GL_FLOAT, GL_FALSE, _positionsDataDimension * sizeof(GLfloat), (void*)offsetSize);
        // location defined in shader
        glEnableVertexAttribArray(0);
    }

    virtual void addPositionsVec4(const std::vector<glm::vec4>& positionsData) {
        GLintptr offsetSize = static_cast<int>(_positionsDataVec4.size()) * _positionsDataDimension * sizeof(float);
        _positionsDataVec4.insert(_positionsDataVec4.end(), positionsData.begin(), positionsData.end());
        _positionsDataCount = static_cast<int>(_positionsDataVec4.size()) * _positionsDataDimension;

        // bind vao
        glBindVertexArray(_VAO);
        // bind vbo
        glBindBuffer(GL_ARRAY_BUFFER, _VBO);
        // calculate position data size
        const auto positionsDataSize = _positionsDataCount * sizeof(GLfloat);
        // set position sub data
        glBufferSubData(GL_ARRAY_BUFFER, offsetSize, positionsDataSize, _positionsDataVec4.data());
        // configure position attribute
        glVertexAttribPointer(0, _positionsDataDimension, GL_FLOAT, GL_FALSE, _positionsDataDimension * sizeof(GLfloat), (void*)offsetSize);
        // location defined in shader
        glEnableVertexAttribArray(0);
    }

    void init() override {
        // generating buffers, binding buffers, storing buffers, configuring attributes, unbinding buffers

        // generate vao
        glGenVertexArrays(1, &_VAO);
        // generate vbo
        glGenBuffers(1, &_VBO);
        if (_entityType == EntityType::RECTANGLE) {
            // generate ebo
            glGenBuffers(1, &_EBO);
        }

        // bind vao
        glBindVertexArray(_VAO);
        // bind vbo
        glBindBuffer(GL_ARRAY_BUFFER, _VBO);
        if (_entityType == EntityType::RECTANGLE) {
            // bind ebo
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
        }

        // calculate total data size
        const auto totalDataSize = _totalDataCount * sizeof(GLfloat);
        // reserve vbo data for entity
        glBufferData(GL_ARRAY_BUFFER, totalDataSize, nullptr, GL_STATIC_DRAW);
        if (_entityType == EntityType::RECTANGLE) {
            // set indices
            const std::vector<GLuint> indices{ 0, 1, 3, 1, 2, 3 };
            // store ebo data
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
        }

        GLintptr offsetSize{ 0 };

        if (_usePositionsVec2) {
            // calculate position data size
            const auto positionsDataSize = _positionsDataCount * sizeof(GLfloat);
            // set position sub data
            glBufferSubData(GL_ARRAY_BUFFER, offsetSize, positionsDataSize, _positionsDataVec2.data());
            // configure position attribute
            glVertexAttribPointer(0, _positionsDataDimension, GL_FLOAT, GL_FALSE, _positionsDataDimension * sizeof(GLfloat), (void*)offsetSize);
            // location defined in shader
            glEnableVertexAttribArray(0);
            // update offset size
            offsetSize += positionsDataSize;
        }

        if (_usePositions) {
            // calculate position data size
            const auto positionsDataSize = _positionsDataCount * sizeof(GLfloat);
            // set position sub data
            glBufferSubData(GL_ARRAY_BUFFER, offsetSize, positionsDataSize, _positionsData.data());
            // configure position attribute
            glVertexAttribPointer(0, _positionsDataDimension, GL_FLOAT, GL_FALSE, _positionsDataDimension * sizeof(GLfloat), (void*)offsetSize);
            // location defined in shader
            glEnableVertexAttribArray(0);
            // update offset size
            offsetSize += positionsDataSize;
        }

        if (_usePositionsVec4) {
            // calculate position data size
            const auto positionsDataSize = _positionsDataCount * sizeof(GLfloat);
            // set position sub data
            glBufferSubData(GL_ARRAY_BUFFER, offsetSize, positionsDataSize, _positionsDataVec4.data());
            // configure position attribute
            glVertexAttribPointer(0, _positionsDataDimension, GL_FLOAT, GL_FALSE, _positionsDataDimension * sizeof(GLfloat), (void*)offsetSize);
            // location defined in shader
            glEnableVertexAttribArray(0);
            // update offset size
            offsetSize += positionsDataSize;
        }

        if (_useColors) {
            // calculate color data size
            const auto colorsDataSize = _colorsDataCount * sizeof(GLfloat);
            // set color sub data
            glBufferSubData(GL_ARRAY_BUFFER, offsetSize, colorsDataSize, _colorsData.data());
            // configure color attribute
            glVertexAttribPointer(1, _colorsDataDimension, GL_FLOAT, GL_FALSE, _colorsDataDimension * sizeof(GLfloat), (void*)offsetSize);
            // location defined in shader
            glEnableVertexAttribArray(1);
            // update offset size
            offsetSize += colorsDataSize;
        }

        if (_useTexturePositions) {
            // calculate texture data size
            const auto texturePositionsDataSize = _texturePositionsDataCount * sizeof(GLfloat);
            // set texture sub data
            glBufferSubData(GL_ARRAY_BUFFER, offsetSize, texturePositionsDataSize, _texturePositionsData.data());
            // configure texture attribute
            glVertexAttribPointer(2, _texturePositionsDataDimension, GL_FLOAT, GL_FALSE, _texturePositionsDataDimension * sizeof(GLfloat), (void*)offsetSize);
            // location defined in shader
            glEnableVertexAttribArray(2);
            // update offset size
            offsetSize += texturePositionsDataSize;
        }

        if (_useNormals) {
            // calculate normal data size
            const auto normalsDataSize = _normalsDataCount * sizeof(GLfloat);
            // set normal sub data
            glBufferSubData(GL_ARRAY_BUFFER, offsetSize, normalsDataSize, _normalsData.data());
            // configure normal attribute
            glVertexAttribPointer(3, _normalsDataDimension, GL_FLOAT, GL_FALSE, _normalsDataDimension * sizeof(GLfloat), (void*)offsetSize);
            // location defined in shader
            glEnableVertexAttribArray(3);
            // update offset size
            offsetSize += normalsDataSize;
        }

        // unbind vao
        glBindVertexArray(0);
        // unbind vbo
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void update() override {
        if (_camera) {
            getShader().setMat4("u_view", _camera->getView());
        }
    }

    const std::vector<glm::vec3>& getPositionsData() {
        return _positionsData;
    }

    Color& getColor() {
        return _color;
    }

    Shader& getShader() {
        _shader.use();
        return _shader;
    }

    Transform& getTransform() {
        return _transform;
    }

protected:
    ICamera* _camera = nullptr;
    Transform _transform;
    Color _color;
    Shader _shader;
    Texture _texture;
    EntityType _entityType{ EntityType::UNKNOWN };

    int _drawMode{ GL_TRIANGLES };

    unsigned int _VAO{ 0 };
    unsigned int _VBO{ 0 };
    unsigned int _EBO{ 0 };

    std::vector<glm::vec2> _positionsDataVec2;
    std::vector<glm::vec3> _positionsData;
    std::vector<glm::vec4> _positionsDataVec4;
    std::vector<glm::vec4> _colorsData;
    std::vector<glm::vec2> _texturePositionsData;
    std::vector<glm::vec3> _normalsData;

    bool _usePositionsVec2{ false };
    bool _usePositions{ false };
    bool _usePositionsVec4{ false };
    bool _useColors{ false };
    bool _useTexture{ false };
    bool _useTexturePositions{ false };
    bool _useNormals{ false };

    int _positionsDataDimension{ 0 };
    int _colorsDataDimension{ 0 };
    int _texturePositionsDataDimension{ 0 };
    int _normalsDataDimension{ 0 };

    int _positionsDataCount{ 0 };
    int _colorsDataCount{ 0 };
    int _texturePositionsDataCount{ 0 };
    int _normalsDataCount{ 0 };
    int _totalDataCount{ 0 };

private:
    void setTexturePositions(std::vector<glm::vec2> texturePositionsData) {
        if (texturePositionsData.empty()) {
            texturePositionsData = { { 1.0f, 1.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 1.0f } };
        }
        _useTexturePositions = true;
        _texturePositionsData = texturePositionsData;
        _texturePositionsDataDimension = 2;
        _texturePositionsDataCount += static_cast<int>(_texturePositionsData.size()) * _texturePositionsDataDimension;
        _totalDataCount += _texturePositionsDataCount;
    }

    void checkGLError() {
        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            LOGGER(error, "gl error occurred: " << err);
        }
    }

};
