#pragma once

#include "Entity.h"

class Triangle : public Entity {
public:
    Triangle(ICamera* camera) : Entity(camera) {
        _entityType = EntityType::triangle;
    }

    ~Triangle() override {
        glDeleteVertexArrays(1, &_VAO);
        glDeleteBuffers(1, &_VBO);
    }

    void render() override {
        _shader.use();
        _texture.bind();
        glBindVertexArray(_VAO);
        glDrawArrays(_drawMode, 0, _positionsDataCount / _engineDimension);
    }

private:

};
