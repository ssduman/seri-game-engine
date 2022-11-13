#pragma once

#include "Entity.h"

class Line : public Entity {
public:
    Line(ICamera* camera) : Entity(camera) {
        _entityType = EntityType::line;
        setDrawMode(aux::DrawMode::line_loop);
    }

    ~Line() override = default;

    void render() override {
        _shader.use();
        _texture.bind();
        glBindVertexArray(_VAO);
        glDrawArrays(_drawMode, 0, _drawArrayCount);
    }

private:

};
