#pragma once

#include "Entity.h"

class Cube : public Entity {
public:
    Cube(Camera* camera, EntityProperties& cubeProperties) : Entity(camera), _cubeProperties(cubeProperties) {
        _entityType = EntityType::CUBE;
        Entity::setProperties(_cubeProperties);
        _renderCount = static_cast<GLsizei>(_cubeProperties.coordinates.size());
    }

    ~Cube() override {
        glDeleteVertexArrays(1, &_VAO);
        glDeleteBuffers(1, &_VBO);
    }

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
