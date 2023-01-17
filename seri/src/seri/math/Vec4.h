#pragma once

#include "seri/math/Vec3.h"
#include "seri/math/Mat4.h"

#include <cmath>
#include <iostream>
#include <stdexcept>

template <typename T = float>
class Vec4 {
public:
    Vec4() {}

    Vec4(T x_) {
        x = x_;
    }

    Vec4(T x_, T y_) {
        x = x_;
        y = y_;
    }

    Vec4(T x_, T y_, T z_) {
        x = x_;
        y = y_;
        z = z_;
    }

    Vec4(T x_, T y_, T z_, T w_) {
        x = x_;
        y = y_;
        z = z_;
        w = w_;
    }

    Vec4(const Vec3<T>& vec3) {
        x = vec3.x;
        y = vec3.y;
        z = vec3.z;
    }

    Vec4(const Vec4<T>& vec4) {
        x = vec4.x;
        y = vec4.y;
        z = vec4.z;
        w = vec4.w;
    }

    ~Vec4() = default;

    /* vec4[] */

    T& operator[](const int& index) {
        if (index == 0) {
            return x;
        }
        if (index == 1) {
            return y;
        }
        if (index == 2) {
            return z;
        }
        if (index == 3) {
            return w;
        }
        throw std::invalid_argument("invalid index");
    }

    /* std::cout << vec4 */

    friend std::ostream& operator<< (std::ostream& out, const Vec4& vec4) {
        out << "Vec4(" << vec4.x << ", " << vec4.y << ", " << vec4.z << ", " << vec4.w << ")";
        return out;
    }

    /* vec3_lhs + 3.14f; */

    Vec4 operator+(const T& rhs) {
        return Vec4{ x + rhs, y + rhs, z + rhs, w + rhs };
    }

    Vec4 operator-(const T& rhs) {
        return Vec4{ x - rhs, y - rhs, z - rhs, w - rhs };
    }

    Vec4 operator*(const T& rhs) {
        return Vec4{ x * rhs, y * rhs, z * rhs, w * rhs };
    }

    Vec4 operator/(const T& rhs) {
        return Vec4{ x / rhs, y / rhs, z / rhs, w / rhs };
    }

    /* vec4_lhs + vec4_rhs; */

    friend Vec4 operator+(const Vec4& lhs, const Vec4& rhs) {
        return Vec4{ lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w };
    }

    friend Vec4 operator-(const Vec4& lhs, const Vec4& rhs) {
        return Vec4{ lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w };
    }

    friend Vec4 operator*(const Vec4& lhs, const Vec4& rhs) {
        return Vec4{ lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w };
    }

    friend Vec4 operator/(const Vec4& lhs, const Vec4& rhs) {
        return Vec4{ lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z, lhs.w / rhs.w };
    }

    /* vec4_lhs + mat4_rhs; */

    friend Mat4<T> operator+(const Vec4& lhs, const Mat4<T>& rhs) {
        Mat4<T> temp{ rhs };
        temp.rows[0] += lhs;
        temp.rows[1] += lhs;
        temp.rows[2] += lhs;
        temp.rows[3] += lhs;
        return temp;
    }

    friend Mat4<T> operator-(const Vec4& lhs, const Mat4<T>& rhs) {
        Mat4<T> temp{ rhs };
        temp.rows[0] -= lhs;
        temp.rows[1] -= lhs;
        temp.rows[2] -= lhs;
        temp.rows[3] -= lhs;
        return temp;
    }

    friend Vec4<T> operator*(const Vec4& lhs, const Mat4<T>& rhs) {
        Mat4 <T>temp{ rhs };
        return rhs.multiply(lhs);
    }

    friend Mat4<T> operator/(const Vec4& lhs, const Mat4<T>& rhs) {
        Mat4<T> temp{ rhs };
        temp.rows[0] /= lhs;
        temp.rows[1] /= lhs;
        temp.rows[2] /= lhs;
        temp.rows[3] /= lhs;
        return temp;
    }

    /* vec4_lhs += 3.14f; */

    friend Vec4 operator+=(Vec4& lhs, const T& rhs) {
        lhs.x += rhs;
        lhs.y += rhs;
        lhs.z += rhs;
        lhs.w += rhs;
        return lhs;
    }

    friend Vec4 operator-=(Vec4& lhs, const T& rhs) {
        lhs.x -= rhs;
        lhs.y -= rhs;
        lhs.z -= rhs;
        lhs.w -= rhs;
        return lhs;
    }

    friend Vec4 operator*=(Vec4& lhs, const T& rhs) {
        lhs.x *= rhs;
        lhs.y *= rhs;
        lhs.z *= rhs;
        lhs.w *= rhs;
        return lhs;
    }

    friend Vec4 operator/=(Vec4& lhs, const T& rhs) {
        lhs.x /= rhs;
        lhs.y /= rhs;
        lhs.z /= rhs;
        lhs.w /= rhs;
        return lhs;
    }

    /* Vec4_lhs += Vec4_rhs; */

    friend Vec4 operator+=(Vec4& lhs, const Vec4& rhs) {
        lhs.x += rhs.x;
        lhs.y += rhs.y;
        lhs.z += rhs.z;
        lhs.w += rhs.w;
        return lhs;
    }

    friend Vec4 operator-=(Vec4& lhs, const Vec4& rhs) {
        lhs.x -= rhs.x;
        lhs.y -= rhs.y;
        lhs.z -= rhs.z;
        lhs.w -= rhs.w;
        return lhs;
    }

    friend Vec4 operator*=(Vec4& lhs, const Vec4& rhs) {
        lhs.x *= rhs.x;
        lhs.y *= rhs.y;
        lhs.z *= rhs.z;
        lhs.w *= rhs.w;
        return lhs;
    }

    friend Vec4 operator/=(Vec4& lhs, const Vec4& rhs) {
        lhs.x /= rhs.x;
        lhs.y /= rhs.y;
        lhs.z /= rhs.z;
        lhs.w /= rhs.w;
        return lhs;
    }

    /* aux */

    float sum() {
        return static_cast<float>(x) + static_cast<float>(y) + static_cast<float>(z) + static_cast<float>(w);
    }

    float norm() {
        return sqrt(dot(*this));
    }

    Vec4<T> normalize() {
        return (*this) / norm();
    }

    float dot(const Vec4<T>& rhs) {
        return ((*this) * rhs).sum();
    }

    float distance(const Vec4<T>& rhs) {
        return sqrt(dot(*this - rhs));
    }

    float angle(const Vec4<T>& rhs) {
        return dot(rhs) / (norm() * rhs.norm());
    }

    /* util */

    template <typename U = float>
    static glm::vec4 toGLMVec4(Vec4<U> vec4) {
        return glm::vec4{ vec4.x, vec4.y, vec4.z, vec4.w };
    }

    T x = 0;
    T y = 0;
    T z = 0;
    T w = 0;

};
