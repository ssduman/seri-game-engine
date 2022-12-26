#pragma once

#include "CameraProperties.h"
#include "../util/Util.h"
#include "../core/State.h"
#include "../logging/Logger.h"

#include <memory>

class ICamera {
public:
    ICamera(CameraProperties cameraProperties) : _cameraProperties{ cameraProperties } {}

    ICamera(CameraProperties cameraProperties, std::shared_ptr<State> state) : _cameraProperties{ cameraProperties }, _state{ state } {}

    virtual ~ICamera() = default;

    virtual void init() = 0;

    virtual void update() = 0;

    virtual void handleMouse(float xPos, float yPos) {
        if (_state && _state->gameState() != GameState::game) {
            return;
        }

        if (_xPosLast < 0 && _yPosLast < 0) {
            _xPosLast = xPos;
            _yPosLast = yPos;
        }

        const auto deltaX = xPos - _xPosLast;
        const auto deltaY = _yPosLast - yPos;

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

        updateEulerAngles();
        updateView();
    }

    void updateAspect(float aspect) {
        _cameraProperties.aspect = aspect;
        updateProjection();
    }

    const glm::mat4& getModel() {
        return _model;
    }

    const glm::mat4& getView() {
        return _view;
    }

    const glm::mat4& getProjection() {
        return _projection;
    }

    CameraProperties& getCameraProperties() {
        return _cameraProperties;
    }

protected:
    virtual void updateView() {
        _view = glm::lookAt(
            _cameraProperties.position,
            _cameraProperties.position + _cameraProperties.front,
            _cameraProperties.up);
    }

    virtual void updateProjection() {
        _projection = glm::perspective(
            glm::radians(_cameraProperties.fov),
            _cameraProperties.aspect,
            _cameraProperties.near,
            _cameraProperties.far);
    }

    virtual void updateEulerAngles() {
        glm::vec3 eulerAngle{};
        eulerAngle.x = cos(glm::radians(_pitch)) * cos(glm::radians(_yaw));
        eulerAngle.y = sin(glm::radians(_pitch));
        eulerAngle.z = cos(glm::radians(_pitch)) * sin(glm::radians(_yaw));

        _cameraProperties.front = glm::normalize(eulerAngle);
        _cameraProperties.right = glm::normalize(glm::cross(_cameraProperties.front, _cameraProperties.up));
    }

    CameraProperties _cameraProperties;
    std::shared_ptr<State> _state;
    glm::mat4 _model{ 1.0f };
    glm::mat4 _view{};
    glm::mat4 _projection{};

    float _roll{ 0.0f };
    float _pitch{ 0.0f };
    float _yaw{ 90.0f };
    float _xPosLast{ -1.0f };
    float _yPosLast{ -1.0f };

};
