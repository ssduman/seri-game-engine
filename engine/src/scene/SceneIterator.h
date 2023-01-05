#pragma once

#include "Scene.h"

#include <memory>
#include <vector>

struct SceneIterator {
    SceneIterator(std::shared_ptr<IScene>& scene) : _scene(scene) {
        prepareSceneVector(_scene);
    }

    auto begin() {
        return _scenes.rbegin();
    }

    auto end() {
        return _scenes.rend();
    }

private:
    void prepareSceneVector(const std::shared_ptr<IScene>& scene) {
        _scenes.push_back(scene);

        for (const auto& s : scene->getChildren()) {
            prepareSceneVector(s);
        }
    }

    std::shared_ptr<IScene>& _scene;
    std::vector<std::shared_ptr<IScene>> _scenes;

};
