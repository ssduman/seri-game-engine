#pragma once

#include "Entity.h"
#include "Shader.h"

class Rectangle : public Entity {
public:
    Rectangle(int windowWidth, int windowHeight) : Entity(windowWidth, windowHeight) {}

    ~Rectangle() {
        glDeleteVertexArrays(1, &_VAO);
        glDeleteBuffers(1, &_VBO);
        glDeleteBuffers(1, &_EBO);
    }

    void initShader(const std::string& vs_path, const std::string& fs_path) override {
        _shader.init(vs_path, fs_path);
    }

    void setProperties(std::vector<glm::vec3>& viewportCoordinates, std::vector<glm::vec3>& colors) override {
        for (auto& coordinate : viewportCoordinates) {
            viewportToClipCoordinate(coordinate);
        }

        for (auto& color : colors) {
            mapRGBColor(color);
        }

        _viewportCoordinates = viewportCoordinates;
        _colors = colors;

        for (int i = 0; i < 4; i++) {
            _vertices.push_back(viewportCoordinates[i].x);
            _vertices.push_back(viewportCoordinates[i].y);
            _vertices.push_back(viewportCoordinates[i].z);

            _vertices.push_back(colors[i].x);
            _vertices.push_back(colors[i].y);
            _vertices.push_back(colors[i].z);
        }

        for (int i = 0; i < 4; i++) {
            std::cout
                << "["
                << _vertices[i * 6 + 0] << ", " << _vertices[i * 6 + 1] << ", " << _vertices[i * 6 + 2]
                << "], ["
                << _vertices[i * 6 + 3] << ", " << _vertices[i * 6 + 4] << ", " << _vertices[i * 6 + 5]
                << "]\n";
        }
    }

    void init() override {
        // generating buffers, binding buffers, storing buffers, configuring attributes, unbinding buffers

        // generate vao
        glGenBuffers(1, &_VBO);
        // generate vbo
        glGenVertexArrays(1, &_VAO);
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

    void update() override {

    }

    void render() override {
        _shader.bind();
        glBindVertexArray(_VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

private:
    Shader _shader;
    unsigned int _VAO = 0;
    unsigned int _VBO = 0;
    unsigned int _EBO = 0;
    std::vector<GLfloat> _vertices;
    std::vector<glm::vec3> _viewportCoordinates;
    std::vector<glm::vec3> _colors;
    std::vector<GLuint> _indices = {
        0, 1, 3, // first triangle
        1, 2, 3, // second triangle
    };
};
