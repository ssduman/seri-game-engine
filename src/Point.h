#pragma once

#include "Entity.h"

class Point : public Entity {
public:
    Point(EntityProperties& pointProperties) : _pointProperties(pointProperties) {
        _entityType = EntityType::POINT;
        setProperties(_pointProperties);
        _renderCount = (GLsizei)_pointProperties.coordinates.size();
    }

    virtual ~Point() {
        glDeleteVertexArrays(1, &_VAO);
        glDeleteBuffers(1, &_VBO);
    }

    void update() override {}

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
