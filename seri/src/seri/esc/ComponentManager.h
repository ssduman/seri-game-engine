#pragma once

#include "seri/esc/Component.h"
#include "seri/esc/ESCEntity.h"

#include <map>
#include <vector>
#include <memory>
#include <utility>
#include <algorithm>

namespace seri {

    struct IComponentArray {
        virtual ~IComponentArray() = default;

        virtual void addComponent(ESCEntity entity) = 0;

        virtual unsigned int getSize() = 0;

    };

    template<typename T>
    struct ComponentArray : public IComponentArray {
        ~ComponentArray() override = default;

        void addComponent(ESCEntity entity) override {
            _enitityComponents[entity.id] = T{};
        }

        T& getComponent(ESCEntity entity) {
            return _enitityComponents[entity.id];
        }

        unsigned int getSize() override {
            return _enitityComponents.size();
        }

    private:
        std::map<unsigned int, T> _enitityComponents;

    };

    struct ComponentManager {
        template<typename T>
        void registerComponent() {
            const char* typeName = typeid(T).name();
            _componentArray[typeName] = std::make_shared<ComponentArray<T>>();
        }

        template<typename T>
        std::shared_ptr<ComponentArray<T>>& getComponentArray() {
            const char* typeName = typeid(T).name();
            return std::static_pointer_cast<ComponentArray<T>>(_componentArray[typeName]);
        }

    private:
        std::map<const char*, std::shared_ptr<IComponentArray>> _componentArray;

    };
}
