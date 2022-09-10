#pragma once

#include "Entity.h"

class Point : public Entity {
public:
    Point(Camera* camera, EntityProperties& pointProperties) : Entity(camera), _pointProperties(pointProperties) {
        _entityType = EntityType::POINT;
        Point::setProperties(_pointProperties);
        _renderCount = static_cast<GLsizei>(_pointProperties.coordinates.size());
    }

    ~Point() override {
        glDeleteVertexArrays(1, &_VAO);
        glDeleteBuffers(1, &_VBO);
    }

    void render() override {
        _shader.use();
        _texture.bind();
        glBindVertexArray(_VAO);
        glDrawArrays(_pointProperties.drawMode, 0, _renderCount);
    }

private:
    GLsizei _renderCount = 0;
    EntityProperties _pointProperties;
};
