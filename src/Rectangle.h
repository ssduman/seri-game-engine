#pragma once

#include "Entity.h"

class Rectangle : public Entity {
public:
    Rectangle(const WindowProperties& windowProperties, EntityProperties& rectangleProperties) :
        Entity(windowProperties), _rectangleProperties(rectangleProperties) {
        setProperties(_rectangleProperties, _vertices);
    }

    ~Rectangle() {
        glDeleteVertexArrays(1, &_VAO);
        glDeleteBuffers(1, &_VBO);
        glDeleteBuffers(1, &_EBO);
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
        // generate ebo
        glGenBuffers(1, &_EBO);

        // bind vao
        glBindVertexArray(_VAO);
        // bind vbo
        glBindBuffer(GL_ARRAY_BUFFER, _VBO);
        // bind ebo
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);

        // store vbo data
        glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(GLfloat), _vertices.data(), GL_STATIC_DRAW);
        // store ebo data
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(GLuint), _indices.data(), GL_STATIC_DRAW);

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
        _shader.bind();
        glBindVertexArray(_VAO);
        glDrawElements(_rectangleProperties.drawMode, 6, GL_UNSIGNED_INT, 0);
    }

private:
    Shader _shader;
    unsigned int _VAO = 0;
    unsigned int _VBO = 0;
    unsigned int _EBO = 0;
    std::vector<GLfloat> _vertices;
    std::vector<GLuint> _indices = {
        0, 1, 3, // first triangle
        1, 2, 3, // second triangle
    };
    EntityProperties _rectangleProperties;
};
