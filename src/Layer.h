#pragma once

#include "Entity.h"

#include <vector>

class Layer {
public:
    Layer() = default;

    void addLayer(Entity* entity) {
        _layers.push_back(entity);
    }

    void deleteLayer() {}

    std::vector<Entity*> getLayers() {
        return _layers;
    }

private:
    std::vector<Entity*> _layers = {};

};
