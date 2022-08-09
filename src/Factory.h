#pragma once

#include "Util.h"
#include "Entity.h"
#include "Point.h"
#include "Line.h"
#include "Triangle.h"
#include "Rectangle.h"
#include "Circle.h"

#include <stdlib.h>

class Factory {
public:
    virtual ~Factory() = 0;

    static Entity* Create(EntityType entityType) {
        glm::vec3 minColor = glm::vec3{ 0.0f, 0.0f, 0.0f };
        glm::vec3 maxColor = glm::vec3{ 1.0f, 1.0f, 1.0f };

        switch (entityType) {
            case EntityType::POINT:
            {
                auto cx = 0.0f;
                auto cy = 0.0f;
                auto cr = 0.5f;
                auto num_segments = 40;
                std::vector<glm::vec3> pointCoordinates{};
                std::vector<glm::vec3> pointColors{};
                for (int i = 0; i < num_segments; i++) {
                    float theta = 2.0f * PI * float(i) / float(num_segments);
                    float x = cr * cosf(theta);
                    float y = cr * sinf(theta);
                    pointCoordinates.push_back(glm::vec3{ x + cx, y + cy, 0.0f });
                    pointColors.push_back(glm::linearRand(minColor, maxColor));
                };
                EntityProperties pointProperties{ pointCoordinates, pointColors };
                pointProperties.drawMode = GL_POINTS;
                Point* point = new Point(pointProperties);
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
                    { glm::linearRand(minColor, maxColor), glm::linearRand(minColor, maxColor), glm::linearRand(minColor, maxColor) },
                };
                lineProperties.drawMode = GL_LINE_LOOP; // GL_LINES GL_LINE_STRIP GL_LINE_LOOP
                Circle* line = new Circle(lineProperties);
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
                    { glm::linearRand(minColor, maxColor), glm::linearRand(minColor, maxColor), glm::linearRand(minColor, maxColor) },
                };
                Triangle* triangle = new Triangle(triangleProperties);
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
                    { glm::linearRand(minColor, maxColor), glm::linearRand(minColor, maxColor), glm::linearRand(minColor, maxColor), glm::linearRand(minColor, maxColor) },
                };
                Rectangle* rectangle = new Rectangle(rectangleProperties);
                rectangle->initShader("shaders/ex_vs.shader", "shaders/ex_fs.shader");
                rectangle->initTexture("textures/wall1.png");
                rectangle->init();
                rectangle->getShader().use();
                rectangle->getShader().setMat4("u_transform", glm::mat4(1.0f));
                rectangle->getShader().disuse();

                return rectangle;
            }
            case EntityType::CIRCLE:
            {
                auto cx = 0.0f;
                auto cy = 0.0f;
                auto cr = 0.5f;
                auto num_segments = 40;
                std::vector<glm::vec3> circleCoordinates{};
                std::vector<glm::vec3> circleColors{};
                for (int i = 0; i < num_segments; i++) {
                    float theta = 2.0f * PI * float(i) / float(num_segments);
                    float x = cr * cosf(theta);
                    float y = cr * sinf(theta);
                    circleCoordinates.push_back(glm::vec3{ x + cx, y + cy, 0.0f });
                    circleColors.push_back(glm::linearRand(minColor, maxColor));
                };
                EntityProperties circleProperties{ circleCoordinates, circleColors };
                circleProperties.drawMode = GL_TRIANGLE_FAN;
                Circle* circle = new Circle(circleProperties);
                circle->initShader("shaders/ex_vs.shader", "shaders/ex_fs.shader");
                circle->initTexture("textures/passage.png");
                circle->init();
                circle->getShader().use();
                circle->getShader().setMat4("u_transform", glm::mat4(1.0f));
                circle->getShader().disuse();

                return circle;
            }
            default:
            {
                return nullptr;
            }
        }
    }

private:

};
