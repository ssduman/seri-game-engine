#pragma once

struct WindowProperties {
    const char* windowTitle = nullptr;
    bool isFullscreen = false;
    int windowWidth = 1280;
    int windowHeight = 720;
};

class IWindowManager {
public:
    IWindowManager(WindowProperties windowProperties) : _windowProperties(windowProperties) {};

    virtual ~IWindowManager() = default;

    virtual bool init() = 0;

    int getWidth() {
        return _windowProperties.windowWidth;
    }

    int getHeight() {
        return _windowProperties.windowHeight;
    }

    float getWidthF() {
        return static_cast<float>(_windowProperties.windowWidth);
    }

    float getHeightF() {
        return static_cast<float>(_windowProperties.windowHeight);
    }

    float getAspect() {
        return getWidthF() / getHeightF();
    }

protected:
    WindowProperties _windowProperties;

    double _lastFrame{ 0.0 };
    double _deltaTime{ 0.0 };
    double _mouseXPosition{ 0.0 };
    double _mouseYPosition{ 0.0 };

};
