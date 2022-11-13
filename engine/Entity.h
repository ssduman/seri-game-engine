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
    Entity() = delete;

    Entity(ICamera* camera) : _camera(camera) {
        generate();
        bind();
    }

    Entity(Entity&& other) noexcept {
        _VAO = other._VAO;
        _VBO = other._VBO;
        _EBO = other._EBO;
        _camera = other._camera;
        _drawMode = other._drawMode;
        _drawArrayCount = other._drawArrayCount;
        _engineDimension = other._engineDimension;

        _shader = std::move(other._shader);
        _texture = std::move(other._texture);

        _color = other._color;
        _transform = other._transform;
        _entityType = other._entityType;

        other._shouldDeleteThis = false;
    }

    Entity(const Entity& other) = delete;

    Entity& operator=(Entity&& other) noexcept {
        _VAO = other._VAO;
        _VBO = other._VBO;
        _EBO = other._EBO;
        _camera = other._camera;
        _drawMode = other._drawMode;
        _drawArrayCount = other._drawArrayCount;
        _engineDimension = other._engineDimension;

        _shader = std::move(other._shader);
        _texture = std::move(other._texture);

        _color = other._color;
        _transform = other._transform;
        _entityType = other._entityType;

        other._shouldDeleteThis = false;

        return *this;
    }

    Entity& operator=(const Entity& other) = delete;

    ~Entity() override {
        if (_shouldDeleteThis && _VAO != 0) {
            unbind();
            del();
            //LOGGER(verbose, "entity delete succeeded");
            return;
        }
        //LOGGER(verbose, "entity delete skipped");
    }

    virtual void initShader(const std::string& vsCodePath, const std::string& fsCodePath, bool readFromFile = true) {
        _shader.init(vsCodePath, fsCodePath, readFromFile);
    }

    virtual void initTexture(const std::string& texturePath) {
        _texture.init(texturePath);
    }

    virtual void initMVP() {
        _shader.use();
        _shader.setMat4("u_model", glm::mat4{ 1.0f });
        _shader.setMat4("u_view", _camera->getView());
        _shader.setMat4("u_projection", _camera->getProjection());
        _shader.disuse();
    }

    void reserveDataBuffer(GLsizeiptr size) {
        dataBuffer({ size });
    }

    template<typename T>
    void setDataBuffer(aux::Index index, const typename std::vector<T>& vec, GLintptr offset = 0) {
        indexCheck(index, vec);

        dataBuffer({ aux::size(vec), aux::data(vec) });
        attribute({ index, aux::length(vec), (const void*)offset });
    }

    template<typename T>
    void setSubDataBuffer(aux::Index index, const typename std::vector<T>& vec, GLintptr offset) {
        indexCheck(index, vec);

        subdataBuffer({ offset, aux::size(vec), aux::data(vec) });
        attribute({ index, aux::length(vec), (const void*)offset });
    }

    void setDrawMode(aux::DrawMode drawMode) {
        _drawMode = aux::toGLenum(drawMode);
    }

    void setDrawArrayCount(size_t drawArrayCount) {
        _drawArrayCount = static_cast<int>(drawArrayCount);
    }

    void setEngineDimension(aux::Dimension dimension) {
        _engineDimension = aux::toInt(dimension);
    }

    void setPosition(const glm::vec2& position) {
        _shader.use();
        _shader.setVec2("u_position", position);
        _shader.disuse();
    }

    void setPosition(const glm::vec3& position) {
        _shader.use();
        _shader.setVec3("u_position", position);
        _shader.disuse();
    }

    void setColor(const glm::vec4& color) {
        useColor(true);
        _color = { color };
        _shader.use();
        _shader.setVec4("u_color", _color.getColorRGBA());
        _shader.disuse();
    }

    void useColor(bool flag) {
        _shader.use();
        _shader.setBool("u_useColor", true);
        _shader.disuse();
    }

    void useColors(bool flag) {
        _shader.use();
        _shader.setBool("u_useColors", flag);
        _shader.disuse();
    }

    void useTexture(bool flag) {
        _shader.use();
        _shader.setBool("u_useTexture", flag);
        _shader.disuse();
    }

    void dataBuffer(const aux::DataBuffer& dataBuffer) {
        bind();
        glBufferData(dataBuffer.target, dataBuffer.size, dataBuffer.data, dataBuffer.usage);
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

    template<typename T>
    void indexCheck(aux::Index index, const typename std::vector<T>& vec) {
        if (aux::Index::position == index) {
            _drawArrayCount = aux::count(vec);
        }
        if (aux::Index::color == index) {
            useColors(true);
        }
        if (aux::Index::texture == index) {
            useTexture(true);
        }
    }

    Color& getColor() {
        return _color;
    }

    Shader& getShader() {
        _shader.use();
        return _shader;
    }

    Texture& getTexture() {
        return _texture;
    }

    Transform& getTransform() {
        return _transform;
    }

protected:
    ICamera* _camera = nullptr;
    Color _color;
    Shader _shader;
    Texture _texture;
    Transform _transform;
    EntityType _entityType{ EntityType::unknown };

    unsigned int _VAO{ 0 };
    unsigned int _VBO{ 0 };
    unsigned int _EBO{ 0 };

    int _drawArrayCount{ 0 };
    int _engineDimension{ aux::toInt(aux::Dimension::two_d) };
    unsigned int _drawMode{ aux::toGLenum(aux::DrawMode::triangles) };

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
        _VAO = 0;
    }

    void deleteVBO() {
        glDeleteBuffers(1, &_VBO);
        _VBO = 0;
    }

    void deleteEBO() {
        glDeleteBuffers(1, &_EBO);
        _EBO = 0;
    }

    bool _shouldDeleteThis = true;

};
