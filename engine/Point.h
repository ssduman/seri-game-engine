#pragma once

#include "Entity.h"

class Point : public Entity {
public:
    Point(std::shared_ptr<ICamera> camera) : Entity(camera) {
        _entityType = EntityType::point;
        _engineBackend.setDrawMode(aux::DrawMode::points);
    }

    ~Point() override = default;

    void render() override {
        _shader.use();
        _texture.bind();
        _engineBackend.draw();
    }

private:

};
