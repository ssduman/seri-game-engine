#pragma once

#include "Entity.h"
#include "Triangle.h"
#include "Rectangle.h"

class Factory {
public:
    virtual ~Factory() = default;

    static Entity* Create(const WindowProperties& windowProperties, EntityType entityType) {
        switch (entityType) {
            case EntityType::TRIANGLE:
            {
                EntityProperties triangleProperties = {
                    { glm::vec3{ 960, 180, 0 }, glm::vec3{ 320, 180, 0 }, glm::vec3{ 640, 540, 0 } },
                    { glm::vec3{ 255, 0, 0 }, glm::vec3{ 0, 255, 0 }, glm::vec3{ 0, 0, 255 } },
                };
                Triangle* triangle = new Triangle(windowProperties, triangleProperties);
                triangle->initShader("shaders/ex_vs.shader", "shaders/ex_fs.shader");
                triangle->initTexture("textures/passage.png");
                triangle->init();
                triangle->getShader().use();
                triangle->getShader().setMat4("u_transform", glm::mat4(1.0f));

                return triangle;
            }
            case EntityType::RECTANGLE:
            {
                EntityProperties rectangleProperties = {
                    { glm::vec3{ 320, 180, 0 }, glm::vec3{ 320, 540, 0 }, glm::vec3{ 960, 540, 0 }, glm::vec3{ 960, 180, 0 } },
                    { glm::vec3{ 255, 0, 0 }, glm::vec3{ 0, 255, 0 }, glm::vec3{ 0, 0, 255 }, glm::vec3{ 255, 255, 255 } },
                };
                Rectangle* rectangle = new Rectangle(windowProperties, rectangleProperties);
                rectangle->initShader("shaders/ex_vs.shader", "shaders/ex_fs.shader");
                rectangle->initTexture("textures/wall1.png");
                rectangle->init();
                rectangle->getShader().use();
                rectangle->getShader().setMat4("u_transform", glm::mat4(1.0f));

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
        return nullptr;
    }

private:

};
