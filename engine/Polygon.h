#pragma once

#include "Entity.h"

class Polygon : public Entity {
public:
    Polygon(ICamera* camera) : Entity(camera) {
        _entityType = EntityType::polygon;
    }

    ~Polygon() override {
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
