#pragma once

#include "../core/Object.h"
#include "../logging/Logger.h"

#include <memory>
#include <string>
#include <vector>
#include <algorithm>

struct Scene {
    std::string name;
    std::shared_ptr<Scene> parent;
    std::shared_ptr<Object> object;
    std::vector<std::shared_ptr<Scene>> children;
};

class SceneManager {
public:
    void addScene(std::shared_ptr<Scene> scene) {
        if (!_root) {
            LOGGER(verbose, "there is no root, setting this scene with name '" << scene->name << "' as root");
            _root = std::move(scene);
            return;
        }

        addChild(_root, std::move(scene));
    }

    void deleteScene(std::shared_ptr<Scene> scene) {
        if (!_root) {
            LOGGER(error, "there is no root to delete scene with name '" << scene->name << "' as root");
            return;
        }

        deleteChild(_root, std::move(scene));
    }

    const std::shared_ptr<Scene>& getRoot() const {
        return _root;
    }

    static bool isLeaf(std::shared_ptr<Scene> scene) {
        return false;
    }

private:
    void addChild(std::shared_ptr<Scene> scene, std::shared_ptr<Scene> child) {
        auto pred = [name = child->name](const std::shared_ptr<Scene>& c) {
            return c->name == name;
        };
        auto loc = std::find_if(scene->children.begin(), scene->children.end(), pred);
        if (loc != scene->children.end()) {
            LOGGER(error, "child with name " << child->name << " already exists");
            return;
        }

        child->parent = scene;
        scene->children.emplace_back(child);
    }

    void deleteChild(std::shared_ptr<Scene> scene, std::shared_ptr<Scene> child) {
        auto pred = [name = child->name](const std::shared_ptr<Scene>& c) {
            return c->name == name;
        };
        auto loc = std::find_if(scene->children.begin(), scene->children.end(), pred);
        if (loc != scene->children.end()) {
            LOGGER(error, "could not find child with name " << child->name);
            return;
        }

        child->parent.reset();
        scene->children.erase(loc);
    }

    std::shared_ptr<Scene> _root;

};

class SceneIterator {

};
