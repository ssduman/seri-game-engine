#pragma once

#include "../core/Object.h"
#include "../logging/Logger.h"

#include <memory>
#include <string>
#include <vector>
#include <algorithm>

struct SceneComponent {
    SceneComponent(std::string name) : _id(generateId()), _name(std::move(name)) {}

    virtual ~SceneComponent() = default;

    virtual void draw() = 0;

    int getId() {
        return _id;
    }

    const std::shared_ptr<Object> getObject() const {
        return _object;
    }

    void setObject(std::shared_ptr<Object> object) {
        _object = std::move(object);
    }

    const std::weak_ptr<SceneComponent> getParent() const {
        return _parent;
    }

    void setParent(std::weak_ptr<SceneComponent> parent) {
        _parent = std::move(parent);
    }

protected:
    int _id{ 0 };
    std::string _name;

    std::shared_ptr<Object> _object;
    std::weak_ptr<SceneComponent> _parent;

private:
    int generateId() {
        return ++SceneComponent::_id_s;
    }

    inline static int _id_s{ 0 };

};

struct SceneComposite : SceneComponent, std::enable_shared_from_this<SceneComponent> {
    SceneComposite(std::string name) : SceneComponent(std::move(name)) {}

    ~SceneComposite() override = default;

    void draw() override {
        if (_object) {
            _object->display();
        }

        for (auto& child : _children) {
            child->draw();
        }
    }

    bool isLeaf() {
        return !_children.empty() || !_object;
    }

    void add(std::shared_ptr<SceneComponent> child) {
        child->setParent(shared_from_this());
        _children.emplace_back(std::move(child));
    }

    void remove(std::shared_ptr<SceneComponent> child) {
        _children.erase(std::remove(_children.begin(), _children.end(), child), _children.end());
    }

    const std::vector<std::shared_ptr<SceneComponent>>& getChildren() const {
        return _children;
    }

private:
    std::vector<std::shared_ptr<SceneComponent>> _children;

};

