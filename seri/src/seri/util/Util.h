#pragma once

#define PI 3.14159265359f

struct Util {
    static float toDegree(float radian) {
        return radian * (180.0f / PI);
    }

    static float toRadian(float degree) {
        return degree * (PI / 180.0f);
    }

    static float map(float x, float in_min, float in_max, float out_min, float out_max) {
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }
};
