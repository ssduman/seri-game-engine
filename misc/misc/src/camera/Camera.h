#pragma once

#include <core/Seri.h>

class Camera : public ICamera {
public:
    Camera(CameraProperties cameraProperties, std::shared_ptr<State> state) : ICamera(cameraProperties, state) {
        LOGGER(info, "camera init succeeded");
    }

    ~Camera() override {
        LOGGER(info, "camera delete succeeded");
    }

    void init() override {
        updateEulerAngles();
        updateView();
        updateProjection();
    }

    void update() override {}

    void handleInput(float deltaTime, CameraMovement cameraMovement) {
        if (_state->gameState() != GameState::game) {
            return;
        }

        const float movementSpeed = _cameraProperties.speed * deltaTime;

        bool moved = false;

        if (cameraMovement == CameraMovement::forward) {
            moved = true;
            _cameraProperties.position += _cameraProperties.front * movementSpeed;
        }
        if (cameraMovement == CameraMovement::backward) {
            moved = true;
            _cameraProperties.position -= _cameraProperties.front * movementSpeed;
        }
        if (cameraMovement == CameraMovement::left) {
            moved = true;
            _cameraProperties.position -= _cameraProperties.right * movementSpeed;
        }
        if (cameraMovement == CameraMovement::right) {
            moved = true;
            _cameraProperties.position += _cameraProperties.right * movementSpeed;
        }

        if (moved) {
            updateView();
        }
    }

};
