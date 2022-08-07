#pragma once

#include "Entity.h"

#include <deque>

class Layer {
public:
    Layer() = default;

    void addLayer(Entity* entity) {
        _layers.push_front(entity);
    }

    void deleteLayer() {
        if (!_layers.empty()) {
            _layers.pop_front();
        }
    }

    std::deque<Entity*> getLayers() {
        return _layers;
    }

private:
    std::deque<Entity*> _layers{};

};
