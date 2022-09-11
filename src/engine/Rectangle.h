#pragma once

#include "Entity.h"

class Rectangle : public Entity {
public:
    Rectangle(ICamera* camera, EntityProperties& rectangleProperties) : Entity(camera), _rectangleProperties(rectangleProperties) {
        _entityType = EntityType::RECTANGLE;
        Entity::setProperties(_rectangleProperties);
    }

    ~Rectangle() override {
        glDeleteVertexArrays(1, &_VAO);
        glDeleteBuffers(1, &_VBO);
        glDeleteBuffers(1, &_EBO);
    }

    void render() override {
        _shader.use();
        _texture.bind();
        glBindVertexArray(_VAO);
        glDrawElements(_rectangleProperties.drawMode, _renderCount, GL_UNSIGNED_INT, nullptr);
    }

private:
    GLsizei _renderCount = 6;
    EntityProperties _rectangleProperties;
};
