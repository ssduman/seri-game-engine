#pragma once

#include "../engine/Util.h"
#include "../engine/State.h"
#include "../engine/Logger.h"
#include "../engine/ICamera.h"

class Camera : public ICamera {
public:
    Camera(CameraProperties cameraProperties, State* state) : ICamera(cameraProperties, state) {
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

    void updateProjection() override {
        _projection = glm::ortho(0.0f, 800.0f, 0.0f, 800.0f, 0.1f, 100.0f);
    }

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
            updateView();

            _viewUpdated = true;
        }
    }

};
