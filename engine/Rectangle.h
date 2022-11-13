#pragma once

#include "Entity.h"

class Rectangle : public Entity {
public:
    Rectangle(ICamera* camera) : Entity(camera) {
        _entityType = EntityType::rectangle;
        setDrawMode(aux::DrawMode::triangles);
    }

    ~Rectangle() override = default;

    void render() override {
        _shader.use();
        _texture.bind();
        glBindVertexArray(_VAO);
        glDrawElements(_drawMode, _renderCount, GL_UNSIGNED_INT, nullptr);
    }

private:
    GLsizei _renderCount = 6;

};
