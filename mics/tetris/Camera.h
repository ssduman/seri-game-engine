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
        updateProjection();
    }

    void update() override {}

    void updateProjection() override {
        _projection = glm::ortho(0.0f, _cameraProperties.width, 0.0f, _cameraProperties.height);
    }

};
