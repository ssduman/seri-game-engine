#pragma once

#include <glm/glm.hpp>

enum class CameraMovement {
    forward,
    backward,
    left,
    right,
};

struct CameraProperties {
    float fov{ 45.0f };
    float aspect{ 16.0f / 9.0f };
    float near{ 0.1f };
    float far{ 100.0f };

    float speed{ 2.5f };
    float sensitivity{ 0.1f };

    float width{ 1280.0f };
    float height{ 720.0f };

    glm::vec3 up{ 0.0f, 1.0f, 0.0f };
    glm::vec3 right{ 1.0f, 0.0f, 0.0f };
    glm::vec3 front{ 0.0f, 0.0f, -1.0f };
    glm::vec3 position{ 0.0f, 0.0f, -3.0f };
};
