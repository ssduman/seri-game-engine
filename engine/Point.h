#pragma once

#include "Entity.h"

class Point : public Entity {
public:
    Point(ICamera* camera) : Entity(camera) {
        _entityType = EntityType::point;
    }

    ~Point() override {
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
