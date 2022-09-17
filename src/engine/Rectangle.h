#pragma once

#include "Entity.h"

class Rectangle : public Entity {
public:
    Rectangle(ICamera* camera) : Entity(camera) {
        _entityType = EntityType::RECTANGLE;
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
        glDrawElements(_drawMode, _renderCount, GL_UNSIGNED_INT, nullptr);
    }

private:
    GLsizei _renderCount = 6;

};
