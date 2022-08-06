#pragma once

#include "Object.h"

#include <deque>

class Layer {
public:
    Layer() = default;

    void addLayer(Object* entity) {
        _layers.push_front(entity);
    }

    void deleteLayer() {
        if (!_layers.empty()) {
            _layers.pop_front();
        }
    }

    std::deque<Object*> getLayers() {
        return _layers;
    }

private:
    std::deque<Object*> _layers{};

};
