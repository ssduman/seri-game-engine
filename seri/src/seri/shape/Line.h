#pragma once

#include "seri/core/Entity.h"

class Line : public Entity {
public:
    Line(std::shared_ptr<ICamera> camera) : Entity(camera) {
        _entityType = EntityType::line;
        _engineBackend.setDrawMode(aux::DrawMode::line_loop);
    }

    ~Line() override = default;

    void render() override {
        _shader.use();
        _texture.bind();
        _engineBackend.draw();
    }

private:

};
