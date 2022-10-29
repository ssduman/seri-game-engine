#pragma once

#include <GL/glew.h>

namespace aux {
    enum class Dimension {
        two_d = 2,
        three_d = 3,
    };

    enum class DrawMode {
        points = GL_POINTS,
        lines = GL_LINES,
        line_loop = GL_LINE_LOOP,
        line_strip = GL_LINE_STRIP,
        triangles = GL_TRIANGLES,
        triangle_strip = GL_TRIANGLE_STRIP,
        triangle_fan = GL_TRIANGLE_FAN,
    };

    enum class Target {
        vbo = GL_ARRAY_BUFFER,
        ebo = GL_ELEMENT_ARRAY_BUFFER,
    };

    enum class Usage {
        stream_draw = GL_STREAM_DRAW,
        static_draw = GL_STATIC_DRAW,
        dynamic_draw = GL_DYNAMIC_DRAW,
    };

    enum class Access {
        read = GL_READ_ONLY,
        write = GL_WRITE_ONLY,
        read_write = GL_READ_WRITE,
    };

    template <typename Enumeration>
    auto toInt(Enumeration const value) -> typename std::underlying_type<Enumeration>::type {
        return static_cast<typename std::underlying_type<Enumeration>::type>(value);
    }

    template <typename Enumeration>
    auto toGLenum(Enumeration const value) -> typename std::underlying_type<Enumeration>::type {
        return static_cast<typename std::underlying_type<Enumeration>::type>(value);
    }

    struct Attribute {
        Attribute() = default;

        Attribute(GLuint index_, GLint size_, const void* pointer_)
            :
            index(index_),
            size(size_),
            pointer(pointer_),
            stride(size * sizeof(GLfloat)) {}

        GLuint index = 0;
        GLint size = 0;
        GLenum type = GL_FLOAT;
        GLboolean normalized = GL_FALSE;
        GLsizei stride = 0;
        const void* pointer = nullptr;
    };

    struct DataBuffer {
        DataBuffer() = default;

        DataBuffer(int size_, const void* data_)
            :
            size(static_cast<GLsizeiptr>(size_)),
            data(data_) {}

        DataBuffer(size_t size_, const void* data_)
            :
            size(static_cast<GLsizeiptr>(size_)),
            data(data_) {}

        DataBuffer(GLsizeiptr size_, const void* data_)
            :
            size(size_),
            data(data_) {}

        DataBuffer(Target target_, GLsizeiptr size_, const void* data_)
            :
            target(toGLenum(target_)),
            size(size_),
            data(data_) {}

        DataBuffer(Target target_, GLsizeiptr size_, const void* data_, Usage usage_)
            :
            target(toGLenum(target_)),
            size(size_),
            data(data_),
            usage(toGLenum(usage_)) {}

        GLenum target = GL_ARRAY_BUFFER;
        GLsizeiptr size = 0;
        const void* data = nullptr;
        GLenum usage = GL_STATIC_DRAW;
    };

    struct SubDataBuffer {
        SubDataBuffer() = default;

        SubDataBuffer(GLintptr offset_, GLsizeiptr size_, const void* data_)
            :
            offset(offset_),
            size(size_),
            data(data_) {}

        SubDataBuffer(Target target_, GLintptr offset_, GLsizeiptr size_, const void* data_)
            :
            target(toGLenum(target_)),
            offset(offset_),
            size(size_),
            data(data_) {}

        GLenum target = GL_ARRAY_BUFFER;
        GLintptr offset = 0;
        GLsizeiptr size = 0;
        const void* data = nullptr;
    };

    struct MapBuffer {
        MapBuffer() = default;

        MapBuffer(size_t size_, const void* data_)
            :
            size(size_),
            data(data_) {}

        MapBuffer(Target target_, Access access_, size_t size_, const void* data_)
            :
            target(toGLenum(target_)),
            access(toGLenum(access_)),
            size(size_),
            data(data_) {}

        GLenum target = GL_ARRAY_BUFFER;
        GLenum access = GL_WRITE_ONLY;
        size_t size = 0;
        const void* data = nullptr;
    };
}
