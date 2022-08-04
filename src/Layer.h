#pragma once

#include "Object.h"

#include <vector>

class Layer {
public:
    Layer() = default;

    void addLayer(Object* entity) {
        _layers.push_back(entity);
    }

    void deleteLayer() {}

    std::vector<Object*> getLayers() {
        return _layers;
    }

private:
    std::vector<Object*> _layers = {};

};
