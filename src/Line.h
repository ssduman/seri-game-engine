#pragma once

#include "Entity.h"

class Line : public Entity {
public:
    Line(EntityProperties& lineProperties) : _lineProperties(lineProperties) {
        _entityType = EntityType::LINE;
        setProperties(_lineProperties);
        _renderCount = (GLsizei)_lineProperties.coordinates.size();
    }

    virtual ~Line() {
        glDeleteVertexArrays(1, &_VAO);
        glDeleteBuffers(1, &_VBO);
    }

    void update() override {}

    void render() override {
        _shader.use();
        _texture.bind();
        glBindVertexArray(_VAO);
        glDrawArrays(_lineProperties.drawMode, 0, _renderCount);
    }

private:
    GLsizei _renderCount = 0;
    EntityProperties _lineProperties;
};
