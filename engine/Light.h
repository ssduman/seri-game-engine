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

        dataBuffer({ /*size*/ sizeof(positions[0]) * positions.size(), /*data*/ positions.data() });
    }

    void render() override {
        _shader.use();
        glBindVertexArray(_VAO);
        glDrawArrays(_drawMode, 0, _positionsDataCount / 3);
    }

private:

};