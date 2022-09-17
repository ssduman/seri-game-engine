#pragma once

#include "Entity.h"

class Line : public Entity {
public:
    Line(ICamera* camera) : Entity(camera) {
        _entityType = EntityType::LINE;
    }

    ~Line() override {
        glDeleteVertexArrays(1, &_VAO);
        glDeleteBuffers(1, &_VBO);
    }

    void render() override {
        _shader.use();
        _texture.bind();
        glBindVertexArray(_VAO);
        glDrawArrays(_drawMode, 0, _positionsDataCount / 3);
    }

private:

};
