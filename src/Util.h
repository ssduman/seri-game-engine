#pragma once

#define PI 3.14159265359

#include "Vec4.h"
#include "Mat4.h"

//#include <glm/glm.hpp>

#include <iostream>

struct Util {

    //template <typename T>
    //static inline glm::vec4 toGLMVec4(const Vec4<T>& vec4) {
    //    return glm::vec4{ vec4.x, vec4.y, vec4.z, vec4.w };
    //}

    //template <typename T>
    //static inline glm::mat4 toGLMMat4(const Mat4<T>& mat4) {
    //    return glm::mat3{ toGLMVec4<T>(mat4.rows[0]), toGLMVec4<T>(mat4.rows[1]), toGLMVec4<T>(mat4.rows[2]), toGLMVec4<T>(mat4.rows[3]) };
    //}

    static inline float toDegree(float radian) {
        return radian * (180 / PI);
    }

    static inline float toRadian(float degree) {
        return degree * (PI / 180);
    }
};