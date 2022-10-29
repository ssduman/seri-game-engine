#pragma once

#include "Entity.h"

class Cube : public Entity {
public:
    Cube(ICamera* camera) : Entity(camera) {
        _entityType = EntityType::cube;
    }

    ~Cube() override {
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
