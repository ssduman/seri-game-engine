#pragma once

#include <glad/gl.h>

struct IndexManager {
    IndexManager() {
        generate();
    }

    void bind() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexID);
    }

private:
    void generate() {
        glGenBuffers(1, &_indexID);
    }

    void del() {
        glDeleteBuffers(1, &_indexID);
        _indexID = 0;
    }

    unsigned int _indexID{ 0 };
};
