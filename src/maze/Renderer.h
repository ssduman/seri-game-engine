#pragma once

#include "../engine/Entity.h"
#include "../engine/Util.h"

class Renderer : public Entity {
public:
    Renderer(float width, float height, float depth) : Entity(nullptr), _width(width), _height(height), _depth(depth) {
        float w = _width, h = _height, z = _depth;
        _vertices = {
            // coords   texture     normals
            -h, +w, -z, 0.0f, 0.0f, +0.0f, +0.0f, -1.0f, // BACK
            +h, +w, -z, 1.0f, 0.0f, +0.0f, +0.0f, -1.0f,
            +h, -w, -z, 1.0f, 1.0f, +0.0f, +0.0f, -1.0f,
            -h, -w, -z, 0.0f, 1.0f, +0.0f, +0.0f, -1.0f,

            +h, +w, +z, 0.0f, 0.0f, +0.0f, +0.0f, +1.0f, // FRONT
            -h, +w, +z, 1.0f, 0.0f, +0.0f, +0.0f, +1.0f,
            -h, -w, +z, 1.0f, 1.0f, +0.0f, +0.0f, +1.0f,
            +h, -w, +z, 0.0f, 1.0f, +0.0f, +0.0f, +1.0f,

            -h, +w, +z, 0.0f, 0.0f, -1.0f, +0.0f, +0.0f, // LEFT
            -h, +w, -z, 1.0f, 0.0f, -1.0f, +0.0f, +0.0f,
            -h, -w, -z, 1.0f, 1.0f, -1.0f, +0.0f, +0.0f,
            -h, -w, +z, 0.0f, 1.0f, -1.0f, +0.0f, +0.0f,

            +h, +w, -z, 0.0f, 0.0f, +1.0f, +0.0f, +0.0f, // RIGHT
            +h, +w, +z, 1.0f, 0.0f, +1.0f, +0.0f, +0.0f,
            +h, -w, +z, 1.0f, 1.0f, +1.0f, +0.0f, +0.0f,
            +h, -w, -z, 0.0f, 1.0f, +1.0f, +0.0f, +0.0f,

            +h, -w, -z, 0.0f, 0.0f, +0.0f, -1.0f, +0.0f, // BOTTOM
            -h, -w, +z, 1.0f, 1.0f, +0.0f, -1.0f, +0.0f,
            -h, -w, -z, 1.0f, 0.0f, +0.0f, -1.0f, +0.0f,
            +h, -w, +z, 0.0f, 1.0f, +0.0f, -1.0f, +0.0f,

            -h, +w, +z, 0.0f, 0.0f, +0.0f, +1.0f, +0.0f, // TOP
            +h, +w, +z, 1.0f, 0.0f, +0.0f, +1.0f, +0.0f,
            +h, +w, -z, 1.0f, 1.0f, +0.0f, +1.0f, +0.0f,
            -h, +w, -z, 0.0f, 1.0f, +0.0f, +1.0f, +0.0f,
        };

        _indices = {
            0, 1, 3, 1, 2, 3, // BACK
            4, 5, 6, 4, 6, 7, // FRONT
            8, 9, 10, 8, 10, 11, // LEFT
            12, 13, 15, 13, 14, 15, // RIGHT
            17, 18, 19, 16, 19, 18, // BOTTOM
            21, 22, 23, 20, 21, 23  // TOP
        };

        Renderer::init();
    }

    ~Renderer() override {
        glDeleteVertexArrays(1, &_VAO);
        glDeleteBuffers(1, &_VBO);
        glDeleteBuffers(1, &_VBO2);
        glDeleteBuffers(1, &_EBO);
    }

    void setCoordinates(unsigned int count, std::vector<glm::vec3>& coordinates) {
        _count = count;

        glGenBuffers(1, &_VBO2);

        glBindVertexArray(_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, _VBO2);

        glBufferData(GL_ARRAY_BUFFER, _count * 3 * sizeof(GLfloat), coordinates.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(3);
        glVertexAttribDivisor(3, 1);

        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(sizeof(float) * 3));
        glVertexAttribDivisor(4, 1);

        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(sizeof(float) * 6));
        glVertexAttribDivisor(5, 1);

        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(sizeof(float) * 9));
        glVertexAttribDivisor(6, 1);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void init() override {
        glGenVertexArrays(1, &_VAO);
        glGenBuffers(1, &_VBO);
        glGenBuffers(1, &_EBO);

        glBindVertexArray(_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, _VBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);

        glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(GLfloat), _vertices.data(), GL_STATIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(GLuint), _indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void render() override {
        glBindVertexArray(_VAO);
        glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, _count);
        glBindVertexArray(0);
    }

private:
    float _width, _height, _depth;
    unsigned int _VBO2 = 0, _count = 0;

};
