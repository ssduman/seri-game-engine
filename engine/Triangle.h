#pragma once

#include "Entity.h"

class Triangle : public Entity {
public:
    Triangle(ICamera* camera) : Entity(camera) {
        _entityType = EntityType::triangle;
        setDrawMode(aux::DrawMode::triangles);
    }

    ~Triangle() override = default;

    void render() override {
        _shader.use();
        _texture.bind();
        glBindVertexArray(_VAO);
        glDrawArrays(_drawMode, 0, _drawArrayCount);
    }

private:

};
