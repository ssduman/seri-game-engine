#pragma once

#include "Entity.h"

class Circle : public Entity {
public:
    Circle(Camera* camera, EntityProperties& circleProperties) : Entity(camera), _circleProperties(circleProperties) {
        _entityType = EntityType::CIRCLE;
        Circle::setProperties(_circleProperties);
        _renderCount = static_cast<GLsizei>(_circleProperties.coordinates.size());
    }

    ~Circle() override {
        glDeleteVertexArrays(1, &_VAO);
        glDeleteBuffers(1, &_VBO);
    }

    void render() override {
        _shader.use();
        _texture.bind();
        glBindVertexArray(_VAO);
        glDrawArrays(_circleProperties.drawMode, 0, _renderCount);
    }

private:
    GLsizei _renderCount = 0;
    EntityProperties _circleProperties;
};
