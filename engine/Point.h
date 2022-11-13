#pragma once

#include "Entity.h"

class Point : public Entity {
public:
    Point(ICamera* camera) : Entity(camera) {
        _entityType = EntityType::point;
        setDrawMode(aux::DrawMode::points);
    }

    ~Point() override = default;

    void render() override {
        _shader.use();
        _texture.bind();
        glBindVertexArray(_VAO);
        glDrawArrays(_drawMode, 0, _drawArrayCount);
    }

private:

};
