#pragma once

#include <glad/gl.h>

struct VertexManager {
    VertexManager() {
        generate();
    }

    void bind() {
        glBindVertexArray(_vertexID);
    }

private:
    void generate() {
        glGenVertexArrays(1, &_vertexID);
    }

    void del() {
        glDeleteVertexArrays(1, &_vertexID);
        _vertexID = 0;
    }

    unsigned int _vertexID{ 0 };
};
