#pragma once

#include "Util.h"
#include "Entity.h"
#include "Triangle.h"
#include "Rectangle.h"

#include <stdlib.h>

class Factory {
public:
    virtual ~Factory() = default;

    static Entity* Create(const WindowProperties& windowProperties, EntityType entityType) {
        switch (entityType) {
            case EntityType::TRIANGLE:
            {
                EntityProperties triangleProperties = {
                    { glm::vec3{ -0.5f, -0.5f, 0 }, glm::vec3{ 0, 0.5f, 0 }, glm::vec3{ 0.5f, -0.5f, 0 } },
                    { glm::vec3{ glm::linearRand(0.0f, 1.0f), 0, 0 }, glm::vec3{ 0, 1.0f, 0 }, glm::vec3{ 0, 0, 1.0f } },
                };
                Triangle* triangle = new Triangle(windowProperties, triangleProperties);
                triangle->initShader("shaders/ex_vs.shader", "shaders/ex_fs.shader");
                triangle->initTexture("textures/passage.png");
                triangle->init();
                triangle->getShader().use();
                triangle->getShader().setMat4("u_transform", glm::mat4(1.0f));
                triangle->getShader().disuse();

                return triangle;
            }
            case EntityType::RECTANGLE:
            {
                EntityProperties rectangleProperties = {
                    { glm::vec3{ -0.5f, -0.5f, 0 }, glm::vec3{ -0.5f, 0.5f, 0 }, glm::vec3{ 0.5f, 0.5f, 0 }, glm::vec3{ 0.5f, -0.5f, 0 } },
                    { glm::vec3{ glm::linearRand(0.0f, 1.0f), 0, 0 }, glm::vec3{ 0, 1.0f, 0 }, glm::vec3{ 0, 0, 1.0f }, glm::vec3{ 1.0f, 1.0f, 1.0f } },
                };
                Rectangle* rectangle = new Rectangle(windowProperties, rectangleProperties);
                rectangle->initShader("shaders/ex_vs.shader", "shaders/ex_fs.shader");
                rectangle->initTexture("textures/wall1.png");
                rectangle->init();
                rectangle->getShader().use();
                rectangle->getShader().setMat4("u_transform", glm::mat4(1.0f));
                rectangle->getShader().disuse();

                return rectangle;
            }
            case EntityType::UNKNOWN:
            {
                return nullptr;
            }
            default:
            {
                return nullptr;
            }
        }
    }

private:

};
