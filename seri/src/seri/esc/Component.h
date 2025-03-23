#pragma once

#include "seri/random/Random.h"
#include "seri/camera/ICamera.h"
#include "seri/texture/Texture.h"

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include <string>
#include <memory>

namespace seri {
    struct IDComponent {
        IDComponent() : id(Random{}.get()) {}

        unsigned long long id;
    };

    struct TagComponent {
        std::string tag{ "default_tag" };
    };

    struct TransformComponent {
        glm::vec3 position{ 0.0f, 0.0f, 0.0f };
        glm::vec3 rotation{ 0.0f, 0.0f, 0.0f };
        glm::vec3 scale{ 1.0f, 1.0f, 1.0f };

        /*
        glm::mat4 getTransform() {
            return glm::translate(glm::mat4(1.0f), position)
                * glm::toMat4(glm::quat(rotation))
                * glm::scale(glm::mat4(1.0f), scale);
        }
        */
    };

    struct SpriteComponent {
        std::shared_ptr<Texture> texture;
        glm::vec4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
    };

    struct CameraComponent {
        std::shared_ptr <ICamera> camera;
    };

    struct GravityComponent {
        glm::vec3 force{ 0.0f, -10.0f, 0.0f };
    };

    struct RigidBodyComponent {
        glm::vec3 velocity{ 0.0f, 0.0f, 0.0f };
        glm::vec3 acceleration{ 0.0f, 0.0f, 0.0f };
    };

}
