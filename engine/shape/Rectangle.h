#pragma once

#include "../core/Entity.h"

class Rectangle : public Entity {
public:
    Rectangle(std::shared_ptr<ICamera> camera) : Entity(camera) {
        _entityType = EntityType::rectangle;
        _engineBackend.setDrawMode(aux::DrawMode::triangles);
    }

    ~Rectangle() override = default;

    void render() override {
        _shader.use();
        _texture.bind();
        _engineBackend.draw();
    }

private:

};
