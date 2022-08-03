#pragma once

#define PI 3.14159265359

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

struct Util {
    static inline float toDegree(float radian) {
        return radian * (180 / PI);
    }

    static inline float toRadian(float degree) {
        return degree * (PI / 180);
    }
};