#pragma once

#include "../logging/Logger.h"

#include <memory>
#include <string>
#include <vector>
#include <algorithm>

struct SceneAble {
    virtual void draw() = 0;
};

struct IScene {
    virtual ~IScene() = default;

    void addChild(std::shared_ptr<IScene> child) {
        auto pred = [name = child->name](const std::shared_ptr<IScene>& child) {
            return child->name == name;
        };
        auto loc = std::find_if(children.begin(), children.end(), pred);
        if (loc == children.end()) {
            children.emplace_back(child);
        }
        else {
            LOGGER(error, "child with name " << child->name << " already exists");
        }
    }

    void deleteChild(std::shared_ptr<IScene> child) {
        auto pred = [name = child->name](const std::shared_ptr<IScene>& child) {
            return child->name == name;
        };
        auto loc = std::find_if(children.begin(), children.end(), pred);
        if (loc != children.end()) {
            children.erase(loc);
        }
        else {
            LOGGER(error, "could not find child with name " << child->name);
        }
    }

    std::string name;
    std::shared_ptr<IScene> parent;
    std::vector<std::shared_ptr<IScene>> children;
};
