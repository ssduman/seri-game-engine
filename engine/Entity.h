#pragma once

#include "Object.h"
#include "Color.h"
#include "Logger.h"
#include "Shader.h"
#include "ICamera.h"
#include "Texture.h"
#include "Transform.h"
#include "EntityType.h"
#include "AuxiliaryStructs.h"

#include <string>
#include <vector>

class Entity : public Object {
public:
    Entity(ICamera* camera) : _camera(camera) {
        generate();
        bind();
    }

    ~Entity() override {
        unbind();
        del();
    }

    virtual void initShader(const std::string& vsCodePath, const std::string& fsCodePath, bool readFromFile = true) {
        _shader.init(vsCodePath, fsCodePath, readFromFile);
    }

    virtual void initMVP() {
        _shader.use();
        _shader.setMat4("u_model", glm::mat4{ 1.0f });
        _shader.setMat4("u_view", _camera->getView());
        _shader.setMat4("u_projection", _camera->getProjection());
        _shader.disuse();
    }

    void setEngineDimension(aux::Dimension dimension) {
        _engineDimension = aux::toInt(dimension);
    }

    void setDrawMode(aux::DrawMode drawMode) {
        _drawMode = aux::toGLenum(drawMode);
    }

    void setColor(const glm::vec4& color) {
        _shader.use();
        _shader.setBool("u_useColor", true);
        _shader.setVec4("u_color", color);
        _shader.disuse();
    }

    void dataBuffer(const aux::DataBuffer& dataBuffer) {
        bind();
        glBufferData(dataBuffer.target, dataBuffer.size, dataBuffer.data, dataBuffer.usage);
        _positionsDataCount = static_cast<int>(dataBuffer.size) / static_cast<int>(sizeof(float));
        unbind();
    }

    void subdataBuffer(const aux::SubDataBuffer& subDataBuffer) {
        bind();
        glBufferSubData(subDataBuffer.target, subDataBuffer.offset, subDataBuffer.size, subDataBuffer.data);
        unbind();
    }

    void mapBuffer(const aux::MapBuffer& mapBuffer) {
        bind();
        void* ptr = glMapBuffer(mapBuffer.target, mapBuffer.access);
        memcpy(ptr, mapBuffer.data, mapBuffer.size);
        glUnmapBuffer(mapBuffer.target);
        unbind();
    }

    void attribute(const aux::Attribute& attribute, bool doEnable = true) {
        bind();
        glVertexAttribPointer(attribute.index, attribute.size, attribute.type, attribute.normalized, attribute.stride, attribute.pointer);
        if (doEnable) {
            enable(attribute);
        }
        unbind();
    }

    void enable(const aux::Attribute& attribute) {
        glEnableVertexAttribArray(attribute.index);
    }

    void init() override {}

    void update() override {
        if (_camera) {
            getShader().setMat4("u_view", _camera->getView());
        }
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
    Shader _shader;
    Texture _texture;
    Transform _transform;
    EntityType _entityType{ EntityType::unknown };

    unsigned int _VAO{ 0 };
    unsigned int _VBO{ 0 };
    unsigned int _EBO{ 0 };

    int _positionsDataCount{ 0 };
    int _drawMode{ aux::toGLenum(aux::DrawMode::triangles) };
    int _engineDimension{ aux::toInt(aux::Dimension::two_d) };

private:
    void generate() {
        generateVAO();
        generateVBO();
        generateEBO();
    }

    void generateVAO() {
        glGenVertexArrays(1, &_VAO);
    }

    void generateVBO() {
        glGenBuffers(1, &_VBO);
    }

    void generateEBO() {
        glGenBuffers(1, &_EBO);
    }

    void bind() {
        bindVAO();
        bindVBO();
        bindEBO();
    }

    void bindVAO() {
        glBindVertexArray(_VAO);
    }

    void bindVBO() {
        glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    }

    void bindEBO() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
    }

    void unbind() {
        unbindVAO();
        unbindVBO();
        unbindEBO();
    }

    void unbindVAO() {
        glBindVertexArray(0);
    }

    void unbindVBO() {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void unbindEBO() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void del() {
        deleteVAO();
        deleteVBO();
        deleteEBO();
    }

    void deleteVAO() {
        glDeleteVertexArrays(1, &_VAO);
    }

    void deleteVBO() {
        glDeleteBuffers(1, &_VBO);
    }

    void deleteEBO() {
        glDeleteBuffers(1, &_EBO);
    }

};
