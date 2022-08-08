#pragma once

#include "Entity.h"

class Line : public Entity {
public:
    Line(EntityProperties& lineProperties) : _lineProperties(lineProperties) {
        _entityType = EntityType::LINE;
        setProperties(_lineProperties);
        _renderCount = (GLsizei)_lineProperties.viewportCoordinates.size();
    }

    ~Line() {
        glDeleteVertexArrays(1, &_VAO);
        glDeleteBuffers(1, &_VBO);
    }

    void init() override {
        // generating buffers, binding buffers, storing buffers, configuring attributes, unbinding buffers

        // generate vao
        glGenVertexArrays(1, &_VAO);
        // generate vbo
        glGenBuffers(1, &_VBO);

        // bind vao
        glBindVertexArray(_VAO);
        // bind vbo
        glBindBuffer(GL_ARRAY_BUFFER, _VBO);

        // store vbo data
        glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(GLfloat), _vertices.data(), GL_STATIC_DRAW);

        // configure position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, _stride * sizeof(GLfloat), (void*)0);
        // location defined in shader
        glEnableVertexAttribArray(0);

        // configure color attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, _stride * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        // location defined in shader
        glEnableVertexAttribArray(1);

        // configure texture attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0 * sizeof(GLfloat), (void*)(_texStart * sizeof(GLfloat)));
        // location defined in shader
        glEnableVertexAttribArray(2);

        // unbind vao
        glBindVertexArray(0);
        // unbind vbo
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void update() override {}

    void render() override {
        _shader.use();
        _texture.bind();
        glBindVertexArray(_VAO);
        glDrawArrays(_lineProperties.drawMode, 0, _renderCount);
    }

private:
    unsigned int _VAO = 0;
    unsigned int _VBO = 0;
    GLsizei _renderCount = 0;
    EntityProperties _lineProperties;
};
