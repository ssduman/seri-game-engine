#pragma once

#include "Entity.h"

class Rectangle : public Entity {
public:
    Rectangle(EntityProperties& rectangleProperties) : _rectangleProperties(rectangleProperties) {
        _entityType = EntityType::RECTANGLE;
        setProperties(_rectangleProperties);
    }

    virtual ~Rectangle() {
        glDeleteVertexArrays(1, &_VAO);
        glDeleteBuffers(1, &_VBO);
        glDeleteBuffers(1, &_EBO);
    }

    void update() override {}

    void render() override {
        _shader.use();
        _texture.bind();
        glBindVertexArray(_VAO);
        glDrawElements(_rectangleProperties.drawMode, _renderCount, GL_UNSIGNED_INT, 0);
    }

private:
    GLsizei _renderCount = 6;
    EntityProperties _rectangleProperties;
};
