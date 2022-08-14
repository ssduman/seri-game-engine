#pragma once

#include "Util.h"

enum class CameraMovement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
};

struct CameraProperties {
    float fov = 45.0f;
    float aspect = 16.0f / 9.0f;
    float near = 0.1f;
    float far = 100.0f;

    glm::vec3 front{ 0.0f, 0.0f, -1.0f };
    glm::vec3 target{ 0.0f, 0.0f, 0.0f };
    glm::vec3 worldUp{ 0.0f, 1.0f, 0.0f };
    glm::vec3 position{ 0.0f, 0.0f, 3.0f };

    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 direction;

    const float YAW = -90.0f;
    const float PITCH = 0.0f;
    const float SPEED = 2.5f;
    const float SENSITIVITY = 0.1f;
    const float ZOOM = 45.0f;
};

class Camera {
public:
    Camera(CameraProperties cameraProperties) : _cameraProperties{ cameraProperties } {
        _cameraProperties.direction = glm::normalize(_cameraProperties.position - _cameraProperties.target);
        _cameraProperties.right = glm::normalize(glm::cross(_cameraProperties.worldUp, _cameraProperties.direction));
        _cameraProperties.up = glm::normalize(glm::cross(_cameraProperties.direction, _cameraProperties.right));

        _view = glm::lookAt(
            _cameraProperties.position,
            _cameraProperties.position + _cameraProperties.front,
            _cameraProperties.up
        );

        _projection = glm::perspective(
            glm::radians(_cameraProperties.fov),
            _cameraProperties.aspect,
            _cameraProperties.near,
            _cameraProperties.far
        );
    }

    ~Camera() = default;

    void handleInput(CameraMovement cameraMovement) {
        float velocity = 0.1f;
        if (cameraMovement == CameraMovement::FORWARD) {
            _cameraProperties.position += _cameraProperties.front * velocity;
        }
        if (cameraMovement == CameraMovement::BACKWARD) {
            _cameraProperties.position -= _cameraProperties.front * velocity;
        }
        if (cameraMovement == CameraMovement::LEFT) {
            _cameraProperties.position -= _cameraProperties.right * velocity;
        }
        if (cameraMovement == CameraMovement::RIGHT) {
            _cameraProperties.position += _cameraProperties.right * velocity;
        }

        view();
    }

    glm::mat4& view() {
        _view = glm::lookAt(
            _cameraProperties.position,
            _cameraProperties.position + _cameraProperties.front,
            _cameraProperties.up
        );
        return _view;
    }

    glm::mat4 _view{};
    glm::mat4 _projection{};
    CameraProperties _cameraProperties;

private:

};
