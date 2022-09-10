#pragma once

#include "Util.h"
#include "State.h"
#include "Logger.h"
#include "ICamera.h"

class Camera : public ICamera {
public:
    Camera(CameraProperties cameraProperties, State* state) : ICamera(cameraProperties, state) {
        Camera::updateVectors();
        Camera::view();
        Camera::projection();

        LOGGER(info, "camera init succeeded");
    }

    ~Camera() override = default;

    void handleInput(float deltaTime, CameraMovement cameraMovement) {
        if (_viewUpdated) {
            _viewUpdated = false;
        }

        if (_state->gameState() != GameState::GAME) {
            return;
        }

        const float movementSpeed = _cameraProperties.speed * deltaTime;

        bool moved = false;

        if (cameraMovement == CameraMovement::FORWARD) {
            moved = true;
            _cameraProperties.position += _cameraProperties.front * movementSpeed;
        }
        if (cameraMovement == CameraMovement::BACKWARD) {
            moved = true;
            _cameraProperties.position -= _cameraProperties.front * movementSpeed;
        }
        if (cameraMovement == CameraMovement::LEFT) {
            moved = true;
            _cameraProperties.position -= _cameraProperties.right * movementSpeed;
        }
        if (cameraMovement == CameraMovement::RIGHT) {
            moved = true;
            _cameraProperties.position += _cameraProperties.right * movementSpeed;
        }

        if (moved) {
            view();

            _viewUpdated = true;
        }
    }

};
