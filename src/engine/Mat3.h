#pragma once

#include "Vec3.h"

#include <cmath>
#include <iostream>
#include <stdexcept>

template <typename T = float>
class Mat3 {
public:
    Mat3() {}

    Mat3(const Vec3<T>& row0, const Vec3<T>& row1, const Vec3<T>& row2) {
        rows[0] = row0;
        rows[1] = row1;
        rows[2] = row2;
    }

    Mat3(const Mat3<T>& rhs) {
        rows[0] = rhs.rows[0];
        rows[1] = rhs.rows[1];
        rows[2] = rhs.rows[2];
    }

    Mat3(T val, bool fill = false) {
        if (!fill) {
            rows[0].x = val;
            rows[1].y = val;
            rows[2].z = val;
        } else {
            rows[0].x = rows[0].y = rows[0].z = val;
            rows[1].x = rows[1].y = rows[1].z = val;
            rows[2].x = rows[2].y = rows[2].z = val;
        }
    }

    ~Mat3() = default;

    /* mat3[] */

    Vec3<T>& operator[](const int& index) {
        if (index < 0 || index > 2) {
            throw std::invalid_argument("invalid index");
        }
        return rows[index];
    }

    /* std::cout << mat3 */

    friend std::ostream& operator<< (std::ostream& out, const Mat3& mat3) {
        out << "Mat3["
            << mat3.rows[0]
            << mat3.rows[1]
            << mat3.rows[2]
            << "]";
        return out;
    }

    /* mat3_lhs + 3.14f; */

    Mat3 operator+(const T& rhs) {
        Mat3 temp{ this };
        temp.rows[0] += rhs;
        temp.rows[1] += rhs;
        temp.rows[2] += rhs;
        return temp;
    }

    Mat3 operator-(const T& rhs) {
        Mat3 temp{ this };
        temp.rows[0] -= rhs;
        temp.rows[1] -= rhs;
        temp.rows[2] -= rhs;
        return temp;
    }

    Mat3 operator*(const T& rhs) {
        Mat3 temp{ this };
        temp.rows[0] *= rhs;
        temp.rows[1] *= rhs;
        temp.rows[2] *= rhs;
        return temp;
    }

    Mat3 operator/(const T& rhs) {
        Mat3 temp{ this };
        temp.rows[0] /= rhs;
        temp.rows[1] /= rhs;
        temp.rows[2] /= rhs;
        return temp;
    }

    /* mat3_lhs + mat3_rhs; */

    friend Mat3 operator+(const Mat3& lhs, const Mat3& rhs) {
        Mat3 temp{ lhs };
        temp.rows[0] += rhs.rows[0];
        temp.rows[1] += rhs.rows[1];
        temp.rows[2] += rhs.rows[2];
        return temp;
    }

    friend Mat3 operator-(const Mat3& lhs, const Mat3& rhs) {
        Mat3 temp{ lhs };
        temp.rows[0] -= rhs.rows[0];
        temp.rows[1] -= rhs.rows[1];
        temp.rows[2] -= rhs.rows[2];
        return temp;
    }

    friend Mat3 operator*(const Mat3& lhs, const Mat3& rhs) {
        Mat3 temp{ lhs };
        temp.rows[0] *= rhs.rows[0];
        temp.rows[1] *= rhs.rows[1];
        temp.rows[2] *= rhs.rows[2];
        return temp;
    }

    friend Mat3 operator/(const Mat3& lhs, const Mat3& rhs) {
        Mat3 temp{ lhs };
        temp.rows[0] /= rhs.rows[0];
        temp.rows[1] /= rhs.rows[1];
        temp.rows[2] /= rhs.rows[2];
        return temp;
    }

    /* mat3_lhs += 3.14f; */

    friend Mat3 operator+=(Mat3& lhs, const T& rhs) {
        lhs.rows[0] += rhs;
        lhs.rows[1] += rhs;
        lhs.rows[2] += rhs;
        return lhs;
    }

    friend Mat3 operator-=(Mat3& lhs, const T& rhs) {
        lhs.rows[0] -= rhs;
        lhs.rows[1] -= rhs;
        lhs.rows[2] -= rhs;
        return lhs;
    }

    friend Mat3 operator*=(Mat3& lhs, const T& rhs) {
        lhs.rows[0] *= rhs;
        lhs.rows[1] *= rhs;
        lhs.rows[2] *= rhs;
        return lhs;
    }

    friend Mat3 operator/=(Mat3& lhs, const T& rhs) {
        lhs.rows[0] /= rhs;
        lhs.rows[1] /= rhs;
        lhs.rows[2] /= rhs;
        return lhs;
    }

    /* mat3_lhs += mat3_rhs; */

    friend Mat3 operator+=(Mat3& lhs, const Mat3& rhs) {
        lhs.rows[0] += rhs.rows[0];
        lhs.rows[1] += rhs.rows[1];
        lhs.rows[2] += rhs.rows[2];
        return lhs;
    }

    friend Mat3 operator-=(Mat3& lhs, const Mat3& rhs) {
        lhs.rows[0] -= rhs.rows[0];
        lhs.rows[1] -= rhs.rows[1];
        lhs.rows[2] -= rhs.rows[2];
        return lhs;
    }

    friend Mat3 operator*=(Mat3& lhs, const Mat3& rhs) {
        lhs.rows[0] *= rhs.rows[0];
        lhs.rows[1] *= rhs.rows[1];
        lhs.rows[2] *= rhs.rows[2];
        return lhs;
    }

    friend Mat3 operator/=(Mat3& lhs, const Mat3& rhs) {
        lhs.rows[0] /= rhs.rows[0];
        lhs.rows[1] /= rhs.rows[1];
        lhs.rows[2] /= rhs.rows[2];
        return lhs;
    }

    /* aux */

    inline Vec3<T> getRow(const int& index) {
        return rows[index];
    }

    inline Vec3<T> getCol(const int& index) {
        return Vec3{ rows[0][index], rows[1][index], rows[2][index] };
    }

    inline Vec3<float> multiply(Vec3<T>& rhs) {
        return Vec3<float>(
            getRow(0).dot(rhs),
            getRow(1).dot(rhs),
            getRow(2).dot(rhs)
        );
    }

    inline Mat3<float> multiply(Mat3<T>& rhs) {
        return Mat3<float>(
            Vec3<float>{ getRow(0).dot(rhs.getCol(0)), getRow(0).dot(rhs.getCol(1)), getRow(0).dot(rhs.getCol(2)) },
            Vec3<float>{ getRow(1).dot(rhs.getCol(0)), getRow(1).dot(rhs.getCol(1)), getRow(1).dot(rhs.getCol(2)) },
            Vec3<float>{ getRow(2).dot(rhs.getCol(0)), getRow(2).dot(rhs.getCol(1)), getRow(2).dot(rhs.getCol(2)) }
        );
    }

    /* data*/

    Vec3<T> rows[3];

};
