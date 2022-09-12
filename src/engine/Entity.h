#pragma once

#include "Object.h"
#include "Color.h"
#include "Logger.h"
#include "Shader.h"
#include "ICamera.h"
#include "Texture.h"
#include "Transform.h"

#include <string>
#include <vector>

enum class EntityType {
    POINT,
    LINE,
    TRIANGLE,
    RECTANGLE,
    CIRCLE,
    CUBE,
    UNKNOWN,
};

inline std::string to_string(EntityType entityType) {
    switch (entityType) {
        case EntityType::POINT:
            return "point";
        case EntityType::LINE:
            return "line";
        case EntityType::TRIANGLE:
            return "triangle";
        case EntityType::RECTANGLE:
            return "rectangle";
        case EntityType::CIRCLE:
            return "circle";
        case EntityType::CUBE:
            return "cube";
        case EntityType::UNKNOWN:
            return "unknown";
        default:
            return "unknown";
    }
}

struct EntityProperties {
    std::vector<glm::vec3> coordinates;
    std::vector<glm::vec3> colors;
    int drawMode = GL_TRIANGLES;

    std::vector<glm::vec2> textureCoordinates{
        glm::vec2{ 1.0f, 1.0f }, glm::vec2{ 1.0f, 0.0f }, glm::vec2{ 0.0f, 0.0f }, glm::vec2{ 0.0f, 1.0f }
    };
};

class Entity : public Object {
public:
    Entity(ICamera* camera) : _camera(camera) {}

    ~Entity() override = default;

    virtual void initShader(const std::string& vsCodePath, const std::string& fsCodePath) {
        _shader.init(vsCodePath, fsCodePath);
        _shader.use();
        _shader.setBool("u_useTexture", false);
        _shader.disuse();
    }

    virtual void initTexture(const std::string& texturePath) {
        _useTexture = true;
        _shader.use();
        _shader.setBool("u_useTexture", true);
        _shader.disuse();
        _texture.init(texturePath);
    }

    virtual void initCamera(ICamera* camera) {
        _shader.use();
        _shader.setMat4("u_model", glm::mat4{ 1.0f });
        _shader.setMat4("u_view", camera->getView());
        _shader.setMat4("u_projection", camera->getProjection());
        _shader.disuse();
    }

    virtual void setProperties(EntityProperties& entityProperties) {
        auto vertexCount = entityProperties.coordinates.size();
        for (size_t i = 0; i < vertexCount; i++) {
            _vertices.push_back(entityProperties.coordinates[i].x);
            _vertices.push_back(entityProperties.coordinates[i].y);
            _vertices.push_back(entityProperties.coordinates[i].z);

            _vertices.push_back(entityProperties.colors[i].x);
            _vertices.push_back(entityProperties.colors[i].y);
            _vertices.push_back(entityProperties.colors[i].z);
        }

        auto textureCount = entityProperties.textureCoordinates.size();
        for (size_t i = 0; i < textureCount; i++) {
            _vertices.push_back(entityProperties.textureCoordinates[i].x);
            _vertices.push_back(entityProperties.textureCoordinates[i].y);
        }

        _texStart = static_cast<int>(_vertices.size() - textureCount * 2);
    }

    virtual void setUseSingleColor(const bool useSingleColor) {
        _useSingleColor = useSingleColor;
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

        // store vbo data for entity
        glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(GLfloat), _vertices.data(), GL_STATIC_DRAW);
        if (_entityType == EntityType::RECTANGLE) {
            // store ebo data
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(GLuint), _indices.data(), GL_STATIC_DRAW);
        }

        // configure position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, _stride * sizeof(GLfloat), (void*)0);
        // location defined in shader
        glEnableVertexAttribArray(0);

        if (_useSingleColor) {
            _shader.use();
            _shader.setVec4("u_color", glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });
            _shader.disuse();
        } else {
            // configure color attribute
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, _stride * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
            // location defined in shader
            glEnableVertexAttribArray(1);
        }

        if (_useTexture) {
            // configure texture attribute
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0 * sizeof(GLfloat), (void*)(_texStart * sizeof(GLfloat)));
            // location defined in shader
            glEnableVertexAttribArray(2);
        }

        // unbind vao
        glBindVertexArray(0);
        // unbind vbo
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void update() override {
        if (_camera && _camera->viewUpdated()) {
            getShader().setMat4("u_view", _camera->getView());
        }
    }

    inline Transform& getTransform() {
        return _transform;
    }

    inline Color& getColor() {
        return _color;
    }

    inline Shader& getShader() {
        _shader.use();
        return _shader;
    }

protected:
    ICamera* _camera;
    Transform _transform;
    Color _color;
    EntityType _entityType = EntityType::UNKNOWN;
    Shader _shader;
    Texture _texture;
    std::vector<GLfloat> _vertices;
    std::vector<GLuint> _indices{ 0, 1, 3, 1, 2, 3 };
    int _stride = 6;
    int _texStart = 0;
    bool _useTexture = false;
    bool _useSingleColor = true;
    unsigned int _VAO = 0;
    unsigned int _VBO = 0;
    unsigned int _EBO = 0;

};
