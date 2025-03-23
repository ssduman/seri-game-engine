#pragma once

namespace seri {
    struct ESCEntity {
        ESCEntity() {
            id = ++_entityId;
        }

        unsigned int id{ 0 };

    private:
        inline static unsigned int _entityId{ 0 };

    };
}
