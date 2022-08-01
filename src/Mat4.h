#pragma once

#include "Vec4.h"

#include <cmath>
#include <iostream>
#include <stdexcept>

template <typename T = float>
class Mat4 {
public:
    Mat4() {}

    Mat4(const Vec4<T>& row0, const Vec4<T>& row1, const Vec4<T>& row2, const Vec4<T>& row3) {
        rows[0] = row0;
        rows[1] = row1;
        rows[2] = row2;
        rows[3] = row3;
    }

    Mat4(const Mat4<T>& rhs) {
        rows[0] = rhs.rows[0];
        rows[1] = rhs.rows[1];
        rows[2] = rhs.rows[2];
        rows[3] = rhs.rows[3];
    }

    Mat4(T val, bool fill = false) {
        if (!fill) {
            rows[0].x = val;
            rows[1].y = val;
            rows[2].z = val;
            rows[3].w = val;
        } else {
            rows[0].x = rows[0].y = rows[0].z = rows[0].w = val;
            rows[1].x = rows[1].y = rows[1].z = rows[1].w = val;
            rows[2].x = rows[2].y = rows[2].z = rows[2].w = val;
            rows[3].x = rows[3].y = rows[3].z = rows[3].w = val;
        }
    }

    ~Mat4() = default;

    /* mat4[] */

    Vec4<T>& operator[](const int& index) {
        if (index < 0 || index > 3) {
            throw std::invalid_argument("invalid index");
        }
        return rows[index];
    }

    /* std::cout << mat4 */

    friend std::ostream& operator<< (std::ostream& out, const Mat4& mat4) {
        out << "Mat4["
            << mat4.rows[0]
            << mat4.rows[1]
            << mat4.rows[2]
            << mat4.rows[3]
            << "]";
        return out;
    }

    /* mat4_lhs + 3.14f; */

    Mat4 operator+(const T& rhs) {
        Mat4 temp{ this };
        temp.rows[0] += rhs;
        temp.rows[1] += rhs;
        temp.rows[2] += rhs;
        temp.rows[3] += rhs;
        return temp;
    }

    Mat4 operator-(const T& rhs) {
        Mat4 temp{ this };
        temp.rows[0] -= rhs;
        temp.rows[1] -= rhs;
        temp.rows[2] -= rhs;
        temp.rows[3] -= rhs;
        return temp;
    }

    Mat4 operator*(const T& rhs) {
        Mat4 temp{ this };
        temp.rows[0] *= rhs;
        temp.rows[1] *= rhs;
        temp.rows[2] *= rhs;
        temp.rows[3] *= rhs;
        return temp;
    }

    Mat4 operator/(const T& rhs) {
        Mat4 temp{ this };
        temp.rows[0] /= rhs;
        temp.rows[1] /= rhs;
        temp.rows[2] /= rhs;
        temp.rows[3] /= rhs;
        return temp;
    }

    /* mat4_lhs + mat4_rhs; */

    friend Mat4 operator+(const Mat4& lhs, const Mat4& rhs) {
        Mat4 temp{ lhs };
        temp.rows[0] += rhs.rows[0];
        temp.rows[1] += rhs.rows[1];
        temp.rows[2] += rhs.rows[2];
        temp.rows[3] += rhs.rows[3];
        return temp;
    }

    friend Mat4 operator-(const Mat4& lhs, const Mat4& rhs) {
        Mat4 temp{ lhs };
        temp.rows[0] -= rhs.rows[0];
        temp.rows[1] -= rhs.rows[1];
        temp.rows[2] -= rhs.rows[2];
        temp.rows[3] -= rhs.rows[3];
        return temp;
    }

    friend Mat4 operator*(const Mat4& lhs, const Mat4& rhs) {
        Mat4 temp{ lhs };
        temp.rows[0] *= rhs.rows[0];
        temp.rows[1] *= rhs.rows[1];
        temp.rows[2] *= rhs.rows[2];
        temp.rows[3] *= rhs.rows[3];
        return temp;
    }

    friend Mat4 operator/(const Mat4& lhs, const Mat4& rhs) {
        Mat4 temp{ lhs };
        temp.rows[0] /= rhs.rows[0];
        temp.rows[1] /= rhs.rows[1];
        temp.rows[2] /= rhs.rows[2];
        temp.rows[3] /= rhs.rows[3];
        return temp;
    }

    /* mat4_lhs += 3.14f; */

    friend Mat4 operator+=(Mat4& lhs, const T& rhs) {
        lhs.rows[0] += rhs;
        lhs.rows[1] += rhs;
        lhs.rows[2] += rhs;
        lhs.rows[3] += rhs;
        return lhs;
    }

    friend Mat4 operator-=(Mat4& lhs, const T& rhs) {
        lhs.rows[0] -= rhs;
        lhs.rows[1] -= rhs;
        lhs.rows[2] -= rhs;
        lhs.rows[3] -= rhs;
        return lhs;
    }

    friend Mat4 operator*=(Mat4& lhs, const T& rhs) {
        lhs.rows[0] *= rhs;
        lhs.rows[1] *= rhs;
        lhs.rows[2] *= rhs;
        lhs.rows[3] *= rhs;
        return lhs;
    }

    friend Mat4 operator/=(Mat4& lhs, const T& rhs) {
        lhs.rows[0] /= rhs;
        lhs.rows[1] /= rhs;
        lhs.rows[2] /= rhs;
        lhs.rows[3] /= rhs;
        return lhs;
    }

    /* mat4_lhs += mat4_rhs; */

    friend Mat4 operator+=(Mat4& lhs, const Mat4& rhs) {
        lhs.rows[0] += rhs.rows[0];
        lhs.rows[1] += rhs.rows[1];
        lhs.rows[2] += rhs.rows[2];
        lhs.rows[3] += rhs.rows[3];
        return lhs;
    }

    friend Mat4 operator-=(Mat4& lhs, const Mat4& rhs) {
        lhs.rows[0] -= rhs.rows[0];
        lhs.rows[1] -= rhs.rows[1];
        lhs.rows[2] -= rhs.rows[2];
        lhs.rows[3] -= rhs.rows[3];
        return lhs;
    }

    friend Mat4 operator*=(Mat4& lhs, const Mat4& rhs) {
        lhs.rows[0] *= rhs.rows[0];
        lhs.rows[1] *= rhs.rows[1];
        lhs.rows[2] *= rhs.rows[2];
        lhs.rows[3] *= rhs.rows[3];
        return lhs;
    }

    friend Mat4 operator/=(Mat4& lhs, const Mat4& rhs) {
        lhs.rows[0] /= rhs.rows[0];
        lhs.rows[1] /= rhs.rows[1];
        lhs.rows[2] /= rhs.rows[2];
        lhs.rows[3] /= rhs.rows[3];
        return lhs;
    }

    /* aux */

    inline Vec4<T> getRow(const int& index) {
        return rows[index];
    }

    inline Vec4<T> getCol(const int& index) {
        return Vec4{ rows[0][index], rows[1][index], rows[2][index], rows[3][index] };
    }

    /* transformations */

    inline Vec4<float> multiply(Vec4<T>& rhs) {
        return Vec4<float>(
            getRow(0).dot(rhs),
            getRow(1).dot(rhs),
            getRow(2).dot(rhs),
            getRow(3).dot(rhs)
        );
    }

    inline Vec4<float> multiply(Vec3<T>& rhs) {
        return Vec4<float>(
            getRow(0).dot(rhs),
            getRow(1).dot(rhs),
            getRow(2).dot(rhs),
            1
        );
    }
    
    inline Vec4<float> scale(Vec3<T>& rhs) {
        return multiply(rhs);
    }

    inline Vec4<float> scale(Vec4<T>& rhs) {
        return multiply(rhs);
    }

    inline Vec4<float> translate(Vec3<T>& rhs) {
        return multiply(rhs);
    }

    inline Vec4<float> translate(Vec4<T>& rhs) {
        return multiply(rhs);
    }

    inline Vec4<float> rotateX() {
        return Vec4{};
    }

    inline Vec4<float> rotateY() {
        return Vec4{};
    }

    inline Vec4<float> rotateZ() {
        return Vec4{};
    }


    /* static transformations */

    inline static Vec4<float> multiply(Mat4<T>& mat4, Vec3<T>& vec3) {
        return Vec4<float>(
            mat4.getRow(0).dot(vec3),
            mat4.getRow(1).dot(vec3),
            mat4.getRow(2).dot(vec3),
            1.0f
        );
    }

    inline static Vec4<float> multiply(Mat4<T>& mat4, Vec4<T>& vec4) {
        return Vec4<float>(
            mat4.getRow(0).dot(vec4),
            mat4.getRow(1).dot(vec4),
            mat4.getRow(2).dot(vec4),
            mat4.getRow(3).dot(vec4)
        );
    }

    inline static Vec4<float> scale(Vec3<T>& vec3, Vec3<T>& scaler) {
        Mat4 mat4{ 1.0f };
        mat4.rows[0].x = scaler[0];
        mat4.rows[1].y = scaler[1];
        mat4.rows[2].z = scaler[2];
        return multiply(mat4, vec3);
    }

    inline static Vec4<float> translate(Vec3<T>& vec3, Vec3<T>& translator) {
        Mat4 mat4{ 1.0f };
        mat4.rows[0].z = translator[0];
        mat4.rows[1].z = translator[1];
        mat4.rows[2].z = translator[2];
        return multiply(mat4, vec3);
    }

    inline static Vec4<float> rotateX(Vec3<T>& vec3, Vec3<T>& rotator) {
        return Vec4{};
    }

    inline static Vec4<float> rotateY(Vec3<T>& vec3, Vec3<T>& rotator) {
        return Vec4{};
    }

    inline static Vec4<float> rotateZ(Vec3<T>& vec3, Vec3<T>& rotator) {
        return Vec4{};
    }

    /* combinations */

    inline Mat4<float> multiply(Mat4<T>& rhs) {
        return Mat4<float>(
            Vec4<float>{ getRow(0).dot(rhs.getCol(0)), getRow(0).dot(rhs.getCol(1)), getRow(0).dot(rhs.getCol(2)), getRow(0).dot(rhs.getCol(3)) },
            Vec4<float>{ getRow(1).dot(rhs.getCol(0)), getRow(1).dot(rhs.getCol(1)), getRow(1).dot(rhs.getCol(2)), getRow(1).dot(rhs.getCol(3)) },
            Vec4<float>{ getRow(2).dot(rhs.getCol(0)), getRow(2).dot(rhs.getCol(1)), getRow(2).dot(rhs.getCol(2)), getRow(2).dot(rhs.getCol(3)) },
            Vec4<float>{ getRow(3).dot(rhs.getCol(0)), getRow(3).dot(rhs.getCol(1)), getRow(3).dot(rhs.getCol(2)), getRow(3).dot(rhs.getCol(3)) }
        );
    }

    /* data*/

    Vec4<T> rows[4];

private:

};
