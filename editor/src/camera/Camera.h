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

    void update() override {
        if (_state->gameState() != GameState::game) {
            return;
        }
        
        auto deltaTime = WindowManagerFactory::instance()->getDeltaTime();
        float movementSpeed = _cameraProperties.speed * deltaTime;

        if (_moveRequest & _wRequest) {
            _cameraProperties.position += _cameraProperties.front * movementSpeed;
        }
        if (_moveRequest & _sRequest) {
            _cameraProperties.position -= _cameraProperties.front * movementSpeed;
        }
        if (_moveRequest & _aRequest) {
            _cameraProperties.position -= _cameraProperties.right * movementSpeed;
        }
        if (_moveRequest & _dRequest) {
            _cameraProperties.position += _cameraProperties.right * movementSpeed;
        }
    }

    void onKeyEvent(const KeyEventData& data) override {
        auto key = data.key;
        auto scancode = data.scancode;
        auto action = data.action;
        auto mods = data.mods;

        if (key == KeyCode::w && action == InputAction::press) {
            _moveRequest |= _wRequest;
        }
        else if (key == KeyCode::w && action == InputAction::release) {
            _moveRequest ^= _wRequest;
        }

        if (key == KeyCode::s && action == InputAction::press) {
            _moveRequest |= _sRequest;
        }
        else if (key == KeyCode::s && action == InputAction::release) {
            _moveRequest ^= _sRequest;
        }

        if (key == KeyCode::a && action == InputAction::press) {
            _moveRequest |= _aRequest;
        }
        else if (key == KeyCode::a && action == InputAction::release) {
            _moveRequest ^= _aRequest;
        }

        if (key == KeyCode::d && action == InputAction::press) {
            _moveRequest |= _dRequest;
        }
        else if (key == KeyCode::d && action == InputAction::release) {
            _moveRequest ^= _dRequest;
        }

        updateView();
    }

private:
    int _wRequest{ 0x0001 };
    int _sRequest{ 0x0010 };
    int _aRequest{ 0x0100 };
    int _dRequest{ 0x1000 };
    int _moveRequest{ 0x0000 };

};
