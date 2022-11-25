#pragma once

#include "Entity.h"

#include <vector>

class Layer {
public:
    Layer() = default;

    void addLayer(std::shared_ptr<Entity> entity) {
        _layers.emplace_back(entity);
    }

    void deleteLayer() {
        if (!_layers.empty()) {
            _layers.pop_back();
        }
    }

    std::vector<std::shared_ptr<Entity>> getLayers() {
        return _layers;
    }

private:
    std::vector<std::shared_ptr<Entity>> _layers{};

};
