#pragma once

#include "Entity.h"

class Circle : public Entity {
public:
    Circle(ICamera* camera) : Entity(camera) {
        _entityType = EntityType::circle;
        setDrawMode(aux::DrawMode::triangle_fan);
    }

    ~Circle() override = default;

    void render() override {
        _shader.use();
        _texture.bind();
        glBindVertexArray(_VAO);
        glDrawArrays(_drawMode, 0, _drawArrayCount);
    }

private:

};
