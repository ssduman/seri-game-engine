#pragma once

#include "Entity.h"

class Triangle : public Entity {
public:
    Triangle(const WindowProperties& windowProperties, EntityProperties& triangleProperties) :
        Entity(windowProperties), _triangleProperties(triangleProperties) {
        setProperties(_triangleProperties, _vertices);
    }

    ~Triangle() {
        glDeleteVertexArrays(1, &_VAO);
        glDeleteBuffers(1, &_VBO);
    }

    void initShader(const std::string& vs_path, const std::string& fs_path) override {
        _shader.init(vs_path, fs_path);
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
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
        // defined in shader -> layout(location = 0) in vec3 aPos;
        glEnableVertexAttribArray(0);

        // configure color attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        // defined in shader -> layout(location = 1) in vec3 aColor;
        glEnableVertexAttribArray(1);

        // unbind vao
        glBindVertexArray(0);
        // unbind vbo
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void update() override {}

    void render() override {
        _shader.use();
        glBindVertexArray(_VAO);
        glDrawArrays(_triangleProperties.drawMode, 0, 3);
    }

private:
    Shader _shader;
    unsigned int _VAO = 0;
    unsigned int _VBO = 0;
    std::vector<GLfloat> _vertices;
    EntityProperties _triangleProperties;
};
