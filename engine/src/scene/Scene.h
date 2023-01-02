#pragma once

#include "../core/Object.h"
#include "../logging/Logger.h"

#include <memory>
#include <string>
#include <vector>
#include <algorithm>

struct Component {
    Component() {
        generateId();
    };

    virtual ~Component() = default;

    virtual void draw() = 0;

    void setObject(std::shared_ptr<Object> object) {
        _object = std::move(object);
    }

    void setParent(std::weak_ptr<Component> parent) {
        _parent = std::move(parent);
    }

protected:
    void generateId() {
        _id = ++Component::_id_s;
    }

    int _id{ 0 };
    inline static int _id_s{ 0 };

    std::shared_ptr<Object> _object;
    std::weak_ptr<Component> _parent;

};

struct Composite : Component, std::enable_shared_from_this<Component>{
    ~Composite() override = default;

    void draw() override {
        if (_object) {
            _object->display();
        }

        for (auto& child : _children) {
            child->draw();
        }
    }

    void add(std::shared_ptr<Component> child) {
        child->setParent(shared_from_this());
        _children.emplace_back(std::move(child));
    }

    void remove(std::shared_ptr<Component> child) {
        _children.erase(std::remove(_children.begin(), _children.end(), child), _children.end());
    }

    const std::vector<std::shared_ptr<Component>>& getChildren() const {
        return _children;
    }

    bool isLeaf() {
        return !_children.empty();
    }

private:
    std::vector<std::shared_ptr<Component>> _children;

};

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
        return !scene->children.empty();
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
