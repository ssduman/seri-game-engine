#pragma once

#include "Entity.h"

class Circle : public Entity {
public:
    Circle(ICamera* camera) : Entity(camera) {
        _entityType = EntityType::circle;
    }

    ~Circle() override {
        glDeleteVertexArrays(1, &_VAO);
        glDeleteBuffers(1, &_VBO);
    }

    void render() override {
        _shader.use();
        _texture.bind();
        glBindVertexArray(_VAO);
        glDrawArrays(_drawMode, 0, _positionsDataCount / _positionsDataDimension);
    }

private:

};
