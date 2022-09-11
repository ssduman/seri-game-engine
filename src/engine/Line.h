#pragma once

#include "Entity.h"

class Line : public Entity {
public:
    Line(ICamera* camera, EntityProperties& lineProperties) : Entity(camera), _lineProperties(lineProperties) {
        _entityType = EntityType::LINE;
        Line::setProperties(_lineProperties);
        _renderCount = static_cast<GLsizei>(_lineProperties.coordinates.size());
    }

    ~Line() override {
        glDeleteVertexArrays(1, &_VAO);
        glDeleteBuffers(1, &_VBO);
    }

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
