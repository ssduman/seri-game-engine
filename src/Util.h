#pragma once

#define PI 3.14159265359f

#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <locale.h>
#include <iostream>

struct Util {
    static inline float toDegree(float radian) {
        return radian * (180.0f / PI);
    }

    static inline float toRadian(float degree) {
        return degree * (PI / 180.0f);
    }
};