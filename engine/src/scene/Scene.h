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
        if (!root) {
            root = std::move(scene);
            return;
        }

        addChild(root, std::move(scene));
    }

    void addChild(std::shared_ptr<Scene> scene, std::shared_ptr<Scene> child) {
        auto pred = [name = child->name](const std::shared_ptr<Scene>& child) {
            return child->name == name;
        };
        auto loc = std::find_if(scene->children.begin(), scene->children.end(), pred);
        if (loc == scene->children.end()) {
            child->parent = scene;
            scene->children.emplace_back(child);
        }
        else {
            LOGGER(error, "child with name " << child->name << " already exists");
        }
    }

    void deleteChild(std::shared_ptr<Scene> scene, std::shared_ptr<Scene> child) {
        auto pred = [name = child->name](const std::shared_ptr<Scene>& child) {
            return child->name == name;
        };
        auto loc = std::find_if(scene->children.begin(), scene->children.end(), pred);
        if (loc == scene->children.end()) {
            child->parent.reset();
            scene->children.erase(loc);
        }
        else {
            LOGGER(error, "could not find child with name " << child->name);
        }
    }

    std::shared_ptr<Scene> getRoot() {
        return root;
    }

private:
    std::shared_ptr<Scene> root;

};

class SceneIterator {

};
