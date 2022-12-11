#pragma once

#include <GL/glew.h>

#include <vector>

namespace aux {
    enum class Dimension : int {
        two_d = 2,
        three_d = 3,
    };

    enum class Index : GLuint {
        position = 0,
        color = 1,
        texture = 2,
        normal = 3,
    };

    enum class Type : GLenum {
        byte_type = GL_BYTE,
        ubyte_type = GL_UNSIGNED_BYTE,
        short_type = GL_SHORT,
        ushort_type = GL_UNSIGNED_SHORT,
        int_type = GL_INT,
        uint_type = GL_UNSIGNED_INT,
        float_type = GL_FLOAT,
        double_type = GL_DOUBLE,
    };

    enum class DrawMode : GLenum {
        points = GL_POINTS,
        lines = GL_LINES,
        line_loop = GL_LINE_LOOP,
        line_strip = GL_LINE_STRIP,
        triangles = GL_TRIANGLES,
        triangle_strip = GL_TRIANGLE_STRIP,
        triangle_fan = GL_TRIANGLE_FAN,
    };

    enum class DrawType : int {
        arrays = 0,
        elements = 1,
    };

    enum class Target : GLenum {
        vbo = GL_ARRAY_BUFFER,
        ebo = GL_ELEMENT_ARRAY_BUFFER,
    };

    enum class Usage : GLenum {
        stream_draw = GL_STREAM_DRAW,
        static_draw = GL_STATIC_DRAW,
        dynamic_draw = GL_DYNAMIC_DRAW,
    };

    enum class Access : GLenum {
        read = GL_READ_ONLY,
        write = GL_WRITE_ONLY,
        read_write = GL_READ_WRITE,
    };

    template<typename T>
    int length(const typename std::vector<T>& vec) {
        return static_cast<int>(T::length());
    }

    template<typename T>
    GLsizei count(const typename std::vector<T>& vec) {
        return static_cast<GLsizei>(vec.size());
    }

    template<typename T>
    GLsizei stride(const typename std::vector<T>& vec) {
        return static_cast<GLsizei>(sizeof(T) * aux::length(vec));
    }

    template<typename T>
    GLsizeiptr size(const typename std::vector<T>& vec) {
        return static_cast<GLsizeiptr>(sizeof(T) * vec.size());
    }

    template<typename T>
    const void* data(const typename std::vector<T>& vec) {
        return vec.data();
    }

    template <typename Enumeration>
    auto toInt(Enumeration const value) -> typename std::underlying_type<Enumeration>::type {
        return static_cast<typename std::underlying_type<Enumeration>::type>(value);
    }

    template <typename Enumeration>
    auto toUInt(Enumeration const value) -> typename std::underlying_type<Enumeration>::type {
        return static_cast<typename std::underlying_type<Enumeration>::type>(value);
    }

    template <typename Enumeration>
    auto toGLenum(Enumeration const value) -> typename std::underlying_type<Enumeration>::type {
        return static_cast<typename std::underlying_type<Enumeration>::type>(value);
    }

    struct Attribute {
        Attribute(Index index_, GLint size_, const void* pointer_)
            :
            index(toUInt(index_)),
            size(size_),
            stride(size * sizeof(GLfloat)),
            pointer(pointer_) {}

        GLuint index = 0;
        GLint size = 0;
        GLenum type = GL_FLOAT;
        GLboolean normalized = GL_FALSE;
        GLsizei stride = 0;
        const void* pointer = nullptr;
    };

    struct DataBuffer {
        DataBuffer(GLsizeiptr size_)
            :
            size(size_) {}

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
