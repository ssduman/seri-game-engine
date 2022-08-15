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
};

class Camera {
public:
    Camera(CameraProperties cameraProperties) : _cameraProperties{ cameraProperties } {
        _cameraProperties.direction = glm::normalize(_cameraProperties.position - _cameraProperties.target);
        _cameraProperties.right = glm::normalize(glm::cross(_cameraProperties.worldUp, _cameraProperties.direction));
        _cameraProperties.up = glm::normalize(glm::cross(_cameraProperties.direction, _cameraProperties.right));

        view();
        projection();
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

    void handleMouse(double xpos, double ypos) {
        float xPos = static_cast<float>(xpos);
        float yPos = static_cast<float>(ypos);

        if (_firstMouse) {
            _lastX = xPos;
            _lastY = yPos;
            _firstMouse = false;
        }

        float xoffset = xPos - _lastX;
        float yoffset = _lastY - yPos;
        _lastX = xPos;
        _lastY = yPos;

        xoffset *= _sensitivity;
        yoffset *= _sensitivity;

        _yaw += xoffset;
        _pitch += yoffset;

        if (_pitch > 89.0f) {
            _pitch = 89.0f;
        }
        if (_pitch < -89.0f) {
            _pitch = -89.0f;
        }

        glm::vec3 front;
        front.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
        front.y = sin(glm::radians(_pitch));
        front.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
        _cameraProperties.front = glm::normalize(front);

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

    glm::mat4& projection() {
        _projection = glm::perspective(
            glm::radians(_cameraProperties.fov),
            _cameraProperties.aspect,
            _cameraProperties.near,
            _cameraProperties.far
        );
        return _projection;
    }

    glm::mat4 _view{};
    glm::mat4 _projection{};
    CameraProperties _cameraProperties;

private:
    bool _firstMouse = false;
    float _lastX = 0;
    float _lastY = 0;
    float _yaw = -90.0f;
    float _pitch = 0.0f;
    float _speed = 2.5f;
    float _sensitivity = 0.1f;
    float _zoom = 45.0f;

};
