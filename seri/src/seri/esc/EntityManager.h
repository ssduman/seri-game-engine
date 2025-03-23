#pragma once

#include "seri/esc/ESCEntity.h"
#include "seri/esc/ComponentManager.h"

namespace seri {
    struct EntityManager {

        static ESCEntity CreateEntity() {
            ESCEntity entity;
            return entity;
        }

    private:
        template<typename T>
        void addComponent(ESCEntity entity) {

        }

        template<typename T>
        T& getComponent(ESCEntity entity) {

        }

        template<typename T>
        void deleteComponent(ESCEntity entity) {

        }

    };
}
