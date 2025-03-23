#pragma once

#include <glad/gl.h>

struct BufferManager {
    BufferManager() {
        generate();
    }

    void bind() {
        glBindBuffer(GL_ARRAY_BUFFER, _bufferID);
    }

private:
    void generate() {
        glGenBuffers(1, &_bufferID);
    }

    void del() {
        glDeleteBuffers(1, &_bufferID);
        _bufferID = 0;
    }

    unsigned int _bufferID{ 0 };
};
