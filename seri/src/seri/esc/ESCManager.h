#pragma once

#include "seri/esc/EntityManager.h"
#include "seri/esc/ComponentManager.h"

namespace seri {
    struct ESCManager {

        friend EntityManager;
        friend ComponentManager;

        ESCManager() {
            registerComponent<seri::IDComponent>();
            registerComponent<seri::TagComponent>();
            registerComponent<seri::TransformComponent>();
            registerComponent<seri::SpriteComponent>();
            registerComponent<seri::CameraComponent>();
            registerComponent<seri::GravityComponent>();
            registerComponent<seri::RigidBodyComponent>();
        }

        ESCEntity createEntity() {
            auto entity = EntityManager::CreateEntity();
            addComponent<seri::IDComponent>(entity);
            addComponent<seri::TagComponent>(entity);
            addComponent<seri::TransformComponent>(entity);

            return entity;
        }

        template<typename T>
        void registerComponent() {
            componentManager.registerComponent<T>();
        }

        template<typename T>
        void addComponent(ESCEntity entity) {
            componentManager.getComponentArray<T>()->addComponent(entity);
        }

        template<typename T>
        T& getComponent(ESCEntity entity) {
            return componentManager.getComponentArray<T>()->getComponent(entity);
        }

        template<typename T>
        void deleteComponent(ESCEntity entity) {

        }


    private:
        EntityManager entityManager;
        ComponentManager componentManager;
    };
}
