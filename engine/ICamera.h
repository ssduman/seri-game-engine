#pragma once

#include "Util.h"
#include "State.h"
#include "Logger.h"
#include "CameraProperties.h"

class ICamera {
public:
    ICamera(CameraProperties cameraProperties) : _cameraProperties{ cameraProperties } {}

    ICamera(CameraProperties cameraProperties, State* state) : _cameraProperties{ cameraProperties }, _state{ state } {}

    virtual ~ICamera() = default;

    virtual void init() = 0;

    virtual void update() = 0;

    void initShader(const std::string& vsCodePath, const std::string& fsCodePath) {
        _shader.init(vsCodePath, fsCodePath);
    }

    virtual void handleMouse(float xPos, float yPos) {
        if (_viewUpdated) {
            _viewUpdated = false;
        }

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

        _viewUpdated = true;
    }

    virtual const bool& isViewUpdated() {
        return _viewUpdated;
    }

    virtual const glm::mat4& getModel() {
        return _model;
    }

    virtual const glm::mat4& getView() {
        return _view;
    }

    virtual const glm::mat4& getProjection() {
        return _projection;
    }

    virtual CameraProperties& getCameraProperties() {
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
    State* _state = nullptr;
    Shader _shader;
    glm::mat4 _model{ 1.0f };
    glm::mat4 _view{};
    glm::mat4 _projection{};

    float _roll{ 0.0f };
    float _pitch{ 0.0f };
    float _yaw{ 90.0f };
    float _xPosLast{ -1.0f };
    float _yPosLast{ -1.0f };
    bool _viewUpdated{ false };
};
