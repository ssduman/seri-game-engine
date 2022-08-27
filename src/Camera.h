#pragma once

#include "Util.h"
#include "Logger.h"
#include "State.h"

enum class CameraMovement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
};

struct CameraProperties {
    float fov{ 45.0f };
    float near{ 0.1f };
    float far{ 100.0f };
    float aspect{ 16.0f / 9.0f };

    float speed{ 2.5f };
    float sensitivity{ 0.1f };

    glm::vec3 up{ 0.0f, 1.0f, 0.0f };
    glm::vec3 right{ 1.0f, 0.0f, 0.0f };
    glm::vec3 front{ 0.0f, 0.0f, -1.0f };
    glm::vec3 position{ 0.0f, 0.0f, -3.0f };
};

inline std::string to_string(CameraMovement cameraMovement) {
    switch (cameraMovement) {
        case CameraMovement::FORWARD:
            return "forward";
        case CameraMovement::BACKWARD:
            return "backward";
        case CameraMovement::LEFT:
            return "left";
        case CameraMovement::RIGHT:
            return "right";
        default:
            return "unknown";
    }
}

class Camera {
public:
    Camera(CameraProperties cameraProperties, State* state) : _cameraProperties{ cameraProperties }, _state{ state } {
        updateVectors();
        view();
        projection();
    }

    ~Camera() = default;

    void handleInput(float deltaTime, CameraMovement cameraMovement) {
        if (_state->gameState() != GameState::GAME) {
            return;
        }

        float movementSpeed = _cameraProperties.speed * deltaTime;

        //LOGGER(debug, "moving: " << to_string(cameraMovement));

        if (cameraMovement == CameraMovement::FORWARD) {
            _cameraProperties.position += _cameraProperties.front * movementSpeed;
        }
        if (cameraMovement == CameraMovement::BACKWARD) {
            _cameraProperties.position -= _cameraProperties.front * movementSpeed;
        }
        if (cameraMovement == CameraMovement::LEFT) {
            _cameraProperties.position -= _cameraProperties.right * movementSpeed;
        }
        if (cameraMovement == CameraMovement::RIGHT) {
            _cameraProperties.position += _cameraProperties.right * movementSpeed;
        }

        view();
    }

    void handleMouse(float xPos, float yPos) {
        if (_state->gameState() != GameState::GAME) {
            return;
        }

        auto deltaX = xPos - _xPosLast;
        auto deltaY = _yPosLast - yPos;
        if (_xPosLast < 0) {
            deltaX = 0;
            deltaY = 0;
        }

        _xPosLast = xPos;
        _yPosLast = yPos;

        _yaw += deltaX * _cameraProperties.sensitivity;
        _pitch += deltaY * _cameraProperties.sensitivity;

        if (_pitch > 89.0f) {
            _pitch = 89.0f;
        }
        if (_pitch < -89.0f) {
            _pitch = -89.0f;
        }

        while (_yaw < -180.0f) {
            _yaw += 360.0f;
        }
        while (_yaw > 180.0f) {
            _yaw -= 360.0f;
        }

        updateVectors();
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

    CameraProperties& getCameraProperties() {
        return _cameraProperties;
    }

    State* _state;
    glm::mat4 _view{};
    glm::mat4 _projection{};
    CameraProperties _cameraProperties;

private:
    void updateVectors() {
        glm::vec3 eulerAngle{};
        eulerAngle.x = cos(glm::radians(_pitch)) * cos(glm::radians(_yaw));
        eulerAngle.y = sin(glm::radians(_pitch));
        eulerAngle.z = cos(glm::radians(_pitch)) * sin(glm::radians(_yaw));

        _cameraProperties.front = glm::normalize(eulerAngle);
        _cameraProperties.right = glm::normalize(glm::cross(_cameraProperties.front, _cameraProperties.up));
    }

    float _xPosLast = -1.0f;
    float _yPosLast = -1.0f;

    float _roll = 0.0f;
    float _pitch = 0.0f;
    float _yaw = 90.0f;

};
