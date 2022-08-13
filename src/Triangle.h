#pragma once

#include "Entity.h"

class Triangle : public Entity {
public:
    Triangle(EntityProperties& triangleProperties) : _triangleProperties(triangleProperties) {
        _entityType = EntityType::TRIANGLE;
        setProperties(_triangleProperties);
    }

    virtual ~Triangle() {
        glDeleteVertexArrays(1, &_VAO);
        glDeleteBuffers(1, &_VBO);
    }

    void update() override {}

    void render() override {
        _shader.use();
        _texture.bind();
        glBindVertexArray(_VAO);
        glDrawArrays(_triangleProperties.drawMode, 0, _renderCount);
    }

private:
    GLsizei _renderCount = 3;
    EntityProperties _triangleProperties;
};
