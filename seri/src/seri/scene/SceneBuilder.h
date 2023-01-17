#pragma once

#include "Scene.h"

#include <memory>
#include <string>

struct SceneBuilder {
    SceneBuilder() {
        _scene = std::make_shared<SceneComponent>("");
    }

    SceneBuilder& setName(std::string name) {
        _scene->setName(std::move(name));
        return *this;
    }

    SceneBuilder& setObject(std::shared_ptr<Object> object) {
        _scene->setObject(std::move(object));
        return *this;
    }

    SceneBuilder& add(std::shared_ptr<IScene> child) {
        _scene->add(std::move(child));
        return *this;
    }

    std::shared_ptr<IScene> build() {
        auto scene = _scene;
        make();
        return scene;
    }

    SceneBuilder& make() {
        _scene = std::make_shared<SceneComponent>("");
        return *this;
    }

    SceneBuilder& reset() {
        _scene.reset();
        return *this;
    }

private:
    std::shared_ptr<IScene> _scene;

};
