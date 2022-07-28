#pragma once

#include "Entity.h"
#include "Shader.h"

class Triangle : public Entity {
public:
    Triangle(int windowWidth, int windowHeight) : Entity(windowWidth, windowHeight) {}

    ~Triangle() {
        glDeleteBuffers(1, &VBO);
        glDeleteVertexArrays(1, &VAO);
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

        for (int i = 0; i < 3; i++) {
            vertices.push_back(viewportCoordinates[i].x);
            vertices.push_back(viewportCoordinates[i].y);
            vertices.push_back(viewportCoordinates[i].z);

            vertices.push_back(colors[i].x);
            vertices.push_back(colors[i].y);
            vertices.push_back(colors[i].z);
        }

        for (int i = 0; i < 3; i++) {
            std::cout
                << "["
                << vertices[i * 6 + 0] << ", " << vertices[i * 6 + 1] << ", " << vertices[i * 6 + 2]
                << "], ["
                << vertices[i * 6 + 3] << ", " << vertices[i * 6 + 4] << ", " << vertices[i * 6 + 5]
                << "]\n";
        }
    }

    void init() override {
        // generate vao
        glGenBuffers(1, &VBO);
        // generate vbo
        glGenVertexArrays(1, &VAO);
        
        // bind the vao first, then bind vbo, then store vbo buffer, then configure vertex attributes.
        glBindVertexArray(VAO);

        // bind vbo
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        // create and store data
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
        // defined in shader -> layout(location = 0) in vec3 aPos;
        glEnableVertexAttribArray(0);

        // color attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        // defined in shader -> layout(location = 1) in vec3 aColor;
        glEnableVertexAttribArray(1);
        
        // glBindVertexArray(0);
    }

    void update() override {

    }

    void render() override {
        _shader.bind();
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

private:
    Shader _shader;
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    std::vector<GLfloat> vertices;
    std::vector<glm::vec3> _viewportCoordinates;
    std::vector<glm::vec3> _colors;
};
