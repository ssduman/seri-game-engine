#pragma once

#include "Util.h"

struct CameraProperties {
    float fov = 45.0f;
    float aspect = 16.0f / 9.0f;
    float near = 0.1f;
    float far = 100.0f;

    glm::vec3 position{ 0.0f, 0.0f, -3.0f };
};

class Camera {
public:
    Camera(CameraProperties cameraProperties) : _cameraProperties{ cameraProperties } {
        _view = glm::mat4{ 1.0f };
        _view = glm::translate(_view, _cameraProperties.position);

        _projection = glm::perspective(
            glm::radians(_cameraProperties.fov),
            _cameraProperties.aspect,
            _cameraProperties.near,
            _cameraProperties.far
        );
    }

    ~Camera() = default;

    glm::mat4& apply() {
        _mvp = _projection * _view * _model;
        return _mvp;
    }

    glm::mat4 _view{};
    glm::mat4 _projection{};
    CameraProperties _cameraProperties;

private:
    glm::mat4 _mvp{};
    glm::mat4 _model{};

};
