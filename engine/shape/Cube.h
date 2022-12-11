#pragma once

#include "../core/Entity.h"

class Cube : public Entity {
public:
    Cube(std::shared_ptr<ICamera> camera) : Entity(camera) {
        _entityType = EntityType::cube;
        _engineBackend.setDrawMode(aux::DrawMode::triangles);
    }

    ~Cube() override = default;

    void render() override {
        _shader.use();
        _texture.bind();
        _engineBackend.draw();
    }

private:

};
