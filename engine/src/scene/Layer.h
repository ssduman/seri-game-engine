#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

struct Layerable {
    Layerable(std::string layerName_) : layerName(layerName_) {}

    virtual ~Layerable() = default;

    virtual void draw() = 0;

    std::string layerName;
};

class LayerManager {
public:
    void addGuiLayer(std::shared_ptr<Layerable> layer) {
        _guiLayer = layer;
    }

    void deleteGuiLayer(std::shared_ptr<Layerable> layer) {
        _guiLayer.reset();
    }

    void addDebugLayer(std::shared_ptr<Layerable> layer) {
        _debugLayer = layer;
    }

    void deleteDebugLayer(std::shared_ptr<Layerable> layer) {
        _debugLayer.reset();
    }

    void addUserLayer(std::shared_ptr<Layerable> layer) {
        _userLayer = layer;
    }

    void deleteUserLayer(std::shared_ptr<Layerable> layer) {
        _userLayer.reset();
    }

    void addSceneLayer(std::shared_ptr<Layerable> layer) {
        _sceneLayer = layer;
    }

    void deleteSceneLayer(std::shared_ptr<Layerable> layer) {
        _sceneLayer.reset();
    }

    void draw() {
        if (_userLayer) {
            _userLayer->draw();
        }
        if (_sceneLayer) {
            _sceneLayer->draw();
        }
        if (_debugLayer) {
            _debugLayer->draw();
        }
        if (_guiLayer) {
            _guiLayer->draw();
        }
    }

private:

    std::shared_ptr<Layerable> _guiLayer;
    std::shared_ptr<Layerable> _userLayer;
    std::shared_ptr<Layerable> _debugLayer;
    std::shared_ptr<Layerable> _sceneLayer;

};
