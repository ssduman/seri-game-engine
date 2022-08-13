#pragma once

#include "Entity.h"

class Cube : public Entity {
public:
    Cube(EntityProperties& cubeProperties) : _cubeProperties(cubeProperties) {
        _entityType = EntityType::CUBE;
        setProperties(_cubeProperties);
        _renderCount = (GLsizei)_cubeProperties.coordinates.size();
    }

    virtual ~Cube() {
        glDeleteVertexArrays(1, &_VAO);
        glDeleteBuffers(1, &_VBO);
    }

    void update() override {}

    void render() override {
        _shader.use();
        _texture.bind();
        glBindVertexArray(_VAO);
        glDrawArrays(_cubeProperties.drawMode, 0, _renderCount);
    }

private:
    GLsizei _renderCount = 0;
    EntityProperties _cubeProperties;
};
