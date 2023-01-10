#pragma once

#include "CameraProperties.h"
#include "../util/Util.h"
#include "../core/State.h"
#include "../core/Object.h"
#include "../logging/Logger.h"

#include <memory>

class ICamera : public Object {
public:
    ICamera(CameraProperties cameraProperties) : _cameraProperties{ cameraProperties } {}

    ICamera(CameraProperties cameraProperties, std::shared_ptr<State> state) : _cameraProperties{ cameraProperties }, _state{ state } {}

    ~ICamera() override = default;

    void render() override {}

    void onMousePositionEvent(const MousePositionEventData& data) override {
        if (_state && _state->gameState() != GameState::game) {
            return;
        }

        auto xpos = static_cast<float>(data.xpos);
        auto ypos = static_cast<float>(data.ypos);

        if (_xPosLast < 0 && _yPosLast < 0) {
            _xPosLast = xpos;
            _yPosLast = ypos;
        }

        const auto deltaX = xpos - _xPosLast;
        const auto deltaY = _yPosLast - ypos;

        _xPosLast = xpos;
        _yPosLast = ypos;

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

    const glm::mat4& getModel() {
        return _model;
    }

    const glm::mat4& getView() {
        return _view;
    }

    const glm::mat4& getProjection() {
        return _projection;
    }

    const CameraProperties& getCameraProperties() {
        return _cameraProperties;
    }

    void setCameraProperties(CameraProperties cameraProperties) {
        _cameraProperties = std::move(cameraProperties);
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
