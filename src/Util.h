#include "Vec3.h"
#include "Mat3.h"

#include <glm/glm.hpp>

#include <iostream>

template <typename T>
glm::vec3 toGLMVec3(const Vec3<T>& vec3) {
    return glm::vec3{ vec3.x, vec3.y, vec3.z };
}

template <typename T>
glm::mat3 toGLMMat3(const Mat3<T>& mat3) {
    return glm::mat3{ toGLMVec3<T>(mat3.rows[0]), toGLMVec3<T>(mat3.rows[1]), toGLMVec3<T>(mat3.rows[2]) };
}

void test_toGLM() {
    glm::vec3 vec3 = toGLMVec3(Vec3{ 1.1f, 2.2f, 3.3f });
    glm::mat3 mat3 = toGLMMat3(Mat3{ 1.0f });

    std::cout << "vec3("
        << mat3[0].x << ", " << mat3[0].y << ", " << mat3[0].z
        << ")"
        << std::endl;

    std::cout << "mat3[\n"
        << "(" << mat3[0].x << ", " << mat3[0].y << ", " << mat3[0].z << ")\n"
        << "(" << mat3[1].x << ", " << mat3[1].y << ", " << mat3[1].z << ")\n"
        << "(" << mat3[2].x << ", " << mat3[2].y << ", " << mat3[2].z << ")\n"
        << "]"
        << std::endl;
}
