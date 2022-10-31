#pragma once

#include "Util.h"
#include "Entity.h"
#include "ICamera.h"

class Light : public Entity {
public:
    Light(ICamera* camera) : Entity(camera) {
        _entityType = EntityType::cube;

        LOGGER(info, "light init succeeded");
    }

    ~Light() override {
        glDeleteVertexArrays(1, &_VAO);
        glDeleteBuffers(1, &_VBO);

        LOGGER(info, "light delete succeeded");
    }

    void setDefaultPositions() {
        const std::vector<glm::vec3> positions = {
            { -0.5f, -0.5f, -0.5f },
            { 0.5f, -0.5f, -0.5f },
            { 0.5f, 0.5f, -0.5f },
            { 0.5f, 0.5f, -0.5f },
            { -0.5f, 0.5f, -0.5f },
            { -0.5f, -0.5f, -0.5f },

            { -0.5f, -0.5f, 0.5f },
            { 0.5f, -0.5f, 0.5f },
            { 0.5f, 0.5f, 0.5f },
            { 0.5f, 0.5f, 0.5f },
            { -0.5f, 0.5f, 0.5f },
            { -0.5f, -0.5f, 0.5f },

            { -0.5f, 0.5f, 0.5f },
            { -0.5f, 0.5f, -0.5f },
            { -0.5f, -0.5f, -0.5f },
            { -0.5f, -0.5f, -0.5f },
            { -0.5f, -0.5f, 0.5f },
            { -0.5f, 0.5f, 0.5f },

            { 0.5f, 0.5f, 0.5f },
            { 0.5f, 0.5f, -0.5f },
            { 0.5f, -0.5f, -0.5f },
            { 0.5f, -0.5f, -0.5f },
            { 0.5f, -0.5f, 0.5f },
            { 0.5f, 0.5f, 0.5f },

            { -0.5f, -0.5f, -0.5f },
            { 0.5f, -0.5f, -0.5f },
            { 0.5f, -0.5f, 0.5f },
            { 0.5f, -0.5f, 0.5f },
            { -0.5f, -0.5f, 0.5f },
            { -0.5f, -0.5f, -0.5f },

            { -0.5f, 0.5f, -0.5f },
            { 0.5f, 0.5f, -0.5f },
            { 0.5f, 0.5f, 0.5f },
            { 0.5f, 0.5f, 0.5f },
            { -0.5f, 0.5f, 0.5f },
            { -0.5f, 0.5f, -0.5f },
        };
        _positionsDataCount = aux::count(positions);
        dataBuffer({ /*size*/ aux::size(positions), /*data*/ positions.data() });
        attribute({ /*index*/ 0, /*size*/ 3, /*pointer*/ 0 });
    }

    void render() override {
        _shader.use();
        glBindVertexArray(_VAO);
        glDrawArrays(_drawMode, 0, _positionsDataCount);
    }

private:
    int _positionsDataCount{ 0 };

};