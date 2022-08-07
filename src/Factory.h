#pragma once

#include "Util.h"
#include "Entity.h"
#include "Point.h"
#include "Line.h"
#include "Triangle.h"
#include "Rectangle.h"

#include <stdlib.h>

class Factory {
public:
    virtual ~Factory() = default;

    static Entity* Create(const WindowProperties& windowProperties, EntityType entityType) {
        switch (entityType) {
            case EntityType::POINT:
            {
                EntityProperties pointProperties = {
                    { glm::vec3{ -0.5f, -0.5f, 0 }, glm::vec3{ 0, 0.5f, 0 }, glm::vec3{ 0.5f, -0.5f, 0 } },
                    { glm::vec3{ glm::linearRand(0.0f, 1.0f), 0, 0 }, glm::vec3{ 0, 1.0f, 0 }, glm::vec3{ 0, 0, 1.0f } },
                };
                pointProperties.drawMode = GL_POINTS;
                Line* point = new Line(windowProperties, pointProperties);
                point->initShader("shaders/ex_vs.shader", "shaders/ex_fs.shader");
                point->initTexture("textures/passage.png");
                point->init();
                point->getShader().use();
                point->getShader().setMat4("u_transform", glm::mat4(1.0f));
                point->getShader().disuse();

                return point;
            }
            case EntityType::LINE:
            {
                EntityProperties lineProperties = {
                    { glm::vec3{ -0.5f, -0.5f, 0 }, glm::vec3{ 0, 0.5f, 0 }, glm::vec3{ 0.5f, -0.5f, 0 } },
                    { glm::vec3{ glm::linearRand(0.0f, 1.0f), 0, 0 }, glm::vec3{ 0, 1.0f, 0 }, glm::vec3{ 0, 0, 1.0f } },
                };
                lineProperties.drawMode = GL_LINE_LOOP; // GL_LINES GL_LINE_STRIP GL_LINE_LOOP
                Line* line = new Line(windowProperties, lineProperties);
                line->initShader("shaders/ex_vs.shader", "shaders/ex_fs.shader");
                line->initTexture("textures/passage.png");
                line->init();
                line->getShader().use();
                line->getShader().setMat4("u_transform", glm::mat4(1.0f));
                line->getShader().disuse();

                return line;
            }
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
            case EntityType::CIRCLE:
                return nullptr;
            default:
            {
                return nullptr;
            }
        }
    }

private:

};
