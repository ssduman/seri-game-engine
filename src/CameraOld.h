#pragma once

#include "Maze.h"
#include "Shader.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

class CameraOld {
public:
    CameraOld(glm::vec3 positionCamera, float aspect, bool p) {
        shader = new Shader();
        shader->init("shaders/basic_vs.shader", "shaders/basic_fs.shader");
        shader->use();
        shader->setVec3("lightPos", glm::vec3(0, 10, 0));
        shader->setVec3("viewPos", glm::vec3(0, 10, 0));

        play = p;

        perspectiveMatrix = glm::perspective(30.0f, aspect, 0.1f, 1000.0f);

        cameraPosition = positionCamera;
        initCameraPos = cameraPosition;
        cameraPositionTemp = cameraPosition;
        Up = cameraUp;

        updateVectors();

        view = glm::lookAt(cameraPosition, cameraPosition + Front, Up);
        initView = view;
        mvp = glm::mat4(perspectiveMatrix * view);

        update(play);
    }

    void setCameraPos(glm::vec3 pos) {
        if (!cheat) {
            cameraPosition = pos;
            view = initView;
            Pitch = 0.0f; Roll = 0.0f; Yaw = 90.0f;
            updateVectors();
            update(play);
        }
    }

    void mouseControl(GLFWwindow* window, double deltaX, double deltaY) {
        Yaw += (float)deltaX * sensitivity;
        Pitch += (float)deltaY * sensitivity;

        if (Pitch > 89) {
            Pitch = 89;
        }
        if (Pitch < -89) {
            Pitch = -89;
        }

        while (Yaw < -180) {
            Yaw += 360;
        }
        while (Yaw > 180) {
            Yaw -= 360;
        }

        updateVectors();
    }

    void keyboardControl(GLFWwindow* window, Maze** maze, bool& play_, bool& escaping, bool& restart) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }
        if (play_ == false) {
            view = glm::lookAt(cameraPosition, cameraPosition + Front, Up);
            return;
        }

        if ((glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) && (checkE)) {
            escaping = escaping == false ? true : false;
            checkE = false;
        }
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE) {
            checkE = true;
        }

        if ((glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) && (checkR)) {
            setCameraPos(initCameraPos);
            delete* maze;
            *maze = new Maze(cubeThickness, mazeWidth, mazeHeight);
            setWallPos((*maze)->getVerticalWallPosition(), (*maze)->getHorizontalWallPosition());
            restart = true;
            checkR = false;
            escaping = false;
        }
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE) {
            checkR = true;
        }

        if ((glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) && (check)) {
            cheat = (cheat == false) ? true : false;
            check = false;
            movementSpeed = 0.3f;
        }
        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE) {
            check = true;
        }

        if ((glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) && (cheat)) {
            movementSpeed += 0.2f;
        }
        if ((glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) && (cheat)) {
            if (movementSpeed > 0.3f) {
                movementSpeed -= 0.2f;
            }
        }

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            // check one step ahead for invisible wall, block stucking or passing maze wall
            cameraPositionTemp += glm::normalize(projectionVector(Front, Right)) * movementSpeed;
            if ((canIPass(cameraPositionTemp) == false) && (!cheat)) {
                // cannot move, step back old position
                cameraPositionTemp = cameraPosition;
            } else if (canIPass(cameraPosition) && (!cheat)) {
                cameraPosition += glm::normalize(projectionVector(Front, Right)) * movementSpeed;
                cameraPositionTemp = cameraPosition;
            } else if (cheat) {
                cameraPosition += Front * movementSpeed;
                cameraPositionTemp = cameraPosition;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            cameraPositionTemp -= glm::normalize(projectionVector(Front, Right)) * movementSpeed;
            if ((canIPass(cameraPositionTemp) == false) && (!cheat)) {
                cameraPositionTemp = cameraPosition;
            } else if (canIPass(cameraPosition) && (!cheat)) {
                cameraPosition -= glm::normalize(projectionVector(Front, Right)) * movementSpeed;
                cameraPositionTemp = cameraPosition;
            } else if (cheat) {
                cameraPosition -= Front * movementSpeed;
                cameraPositionTemp = cameraPosition;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            cameraPositionTemp -= Right * movementSpeed;
            if ((canIPass(cameraPositionTemp) == false) && (!cheat)) {
                cameraPositionTemp = cameraPosition;
            } else if ((canIPass(cameraPosition)) && (!cheat)) {
                cameraPosition -= Right * (movementSpeed);
                cameraPositionTemp = cameraPosition;
            } else if (cheat) {
                cameraPosition -= Right * (movementSpeed);
                cameraPositionTemp = cameraPosition;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            cameraPositionTemp += Right * movementSpeed;
            if ((canIPass(cameraPositionTemp) == false) && (!cheat)) {
                cameraPositionTemp = cameraPosition;
            } else if ((canIPass(cameraPosition)) && (!cheat)) {
                cameraPosition += Right * (movementSpeed);
                cameraPositionTemp = cameraPosition;
            } else if (cheat) {
                cameraPosition += Right * (movementSpeed);
                cameraPositionTemp = cameraPosition;
            }
        }

        view = glm::lookAt(cameraPosition, cameraPosition + Front, Up);
    }

    void update(bool p) {
        play = p;

        shader->use();

        if (!play) {
            dx += 0.01f;
            if (dx >= 360) dx = 0.0f;

            t = cubeThickness;
            glm::vec3 center = glm::vec3(0.0f, -t / 2, 0.0f);
            glm::mat4 trans = glm::rotate(dx, center);
            glm::mat4 myMatrix = glm::translate(getModel(), glm::vec3(-(mazeWidth / 2) * t * 2, -t / 2, -(mazeHeight / 2) * t * 2));

            shader->setMat4("model", trans * myMatrix);
        } else {
            shader->setMat4("model", getModel());
        }

        shader->setMat4("view", view);
        shader->setMat4("projection", perspectiveMatrix);

        shader->setFloat("ambientS", ambient);
    }

    void setDimensions(float w, float h, float th) {
        mazeWidth = w; mazeHeight = h; cubeThickness = th;
    }

    void setWallPos(std::vector<glm::vec3> vp, std::vector<glm::vec3> hp) {
        verticalWallPosition = vp; horizontalWallPosition = hp;
    }

    glm::vec3 getCameraPos() {
        return cameraPosition;
    }

    glm::mat4 getMVP() {
        return glm::mat4(perspectiveMatrix * view);
    }

    glm::mat4 getModel() {
        return glm::mat4(1.0f);
    }

    glm::mat4 getView() {
        return view;
    }

    void setView(glm::mat4 v) {
        view = v;
    }

    glm::mat4 getProjection() {
        return perspectiveMatrix;
    }

    float getAmbient() {
        return ambient;
    }

    void adjustAmbient(float adj) {
        ambient += adj;
    }

private:
    void updateVectors() {
        glm::vec3 eulerAngle;
        eulerAngle.x = -cos(glm::radians(Pitch)) * cos(glm::radians(Yaw));
        eulerAngle.y = sin(glm::radians(Pitch));
        eulerAngle.z = cos(glm::radians(Pitch)) * sin(glm::radians(Yaw));

        Front = glm::normalize(eulerAngle);
        Right = glm::normalize(glm::cross(Front, cameraUp));
        Up = glm::normalize(glm::cross(Right, Front));
    }

    bool canIPass(glm::vec3 position) {
        float currentX = position.x;
        float currentZ = position.z;

        // coordinat based location to index based location
        float currentXPosAtMaze = (currentX - (-cubeThickness)) / (cubeThickness * 2);
        float currentYPosAtMaze = (currentZ - (-cubeThickness)) / (cubeThickness * 2);

        bool cond1 = currentZ > -cubeThickness; // lower bound
        bool cond2 = currentZ < cubeThickness * 2 * mazeHeight - cubeThickness; // upper bound
        bool cond3 = currentX < cubeThickness * 2 * mazeWidth - cubeThickness;  // right bound
        bool cond4 = currentXPosAtMaze >= 0 && currentXPosAtMaze < mazeWidth;   // legal index
        bool cond5 = currentYPosAtMaze >= 0 && currentYPosAtMaze < mazeHeight;  // legal index

        if (cond1 && cond2 && cond3 && cond4 && cond5) {
            std::vector<glm::vec3>::iterator it1;
            std::vector<glm::vec3>::iterator it2;

            position.x = std::round(position.x);
            position.y = 0.0f;
            position.z = std::round(position.z);

            /*     A 5x5 maze
             +Z
              ^
              |
              |    o  o--o--o--o--o
              |    |              |
              |    o  o--o--o--o  o
              |    |  |        |  |
              |    o  o--o--o  o  o
              |    |        |     |
              |    o  o--o  o--o--o
              |    |  |  |        |
              |    o  o  o--o--o  o
              |    |     |        |
              |    o--o--o--o--o  o
              |              (camera)
            (-t,-t)----------------------> +X

            t is the thickness of the cubes. cubes becomes (t*2)x(t*2)x(t*2) cubes.
            so origin of the schema is middle point of the first rendered cube.
            most lower  left   vertical wall ("|",    red walls) is located at (-t, 0, 0)
            most lower right   vertical wall ("|",    red walls) is located at (-t * (width - 1), 0, 0)
            most lower  left horizontal wall ("--", black walls) is located at (0, 0, -t)
            most lower right horizontal wall ("--", black walls) is located at (0, 0, -t * (width - 2))
            x and z of next set of walls increases by t*2

            at first  row we search (t * c, 0, 0)  for vertical wall,    c -> [-1, width - 1]
            at first  row we search (0, 0, t * c)  for horizontal wall,  c -> [-1, width - 1]
            at second row we search (t * c, 0, 10) for vertical wall,    c -> [-1, width - 1]
            at second row we search (10, 0, t * c) for horizontal wall,  c -> [-1, width - 1]

            so at this point z for vertical walls and x for horizontal walls will become indicator of
            which row we are in.
            */

            glm::vec3 vertical(position.x, 0.0f, (std::round(position.z / 10)) * 10);
            glm::vec3 horizontal((std::round(position.x / 10)) * 10, 0.0f, position.z);

            it1 = std::find(verticalWallPosition.begin(), verticalWallPosition.end(), vertical);
            it2 = std::find(horizontalWallPosition.begin(), horizontalWallPosition.end(), horizontal);
            if ((it1 == verticalWallPosition.end()) && (it2 == horizontalWallPosition.end())) {
                return true;
            }
        } else {
            return true;
        }

        return false;
    }

    glm::vec3 projectionVector(glm::vec3 front, glm::vec3 right) {
        // using for walking same speed for any angles
        glm::vec3 u = glm::cross(cameraUp, right);
        glm::vec3 v = front;

        return ((u * v) / float(pow(glm::length(u), 2)) * u);
    }

    Shader* shader;

    glm::mat4 perspectiveMatrix;
    glm::mat4 view, initView;
    glm::mat4 mvp;

    glm::vec3 cameraPosition;
    glm::vec3 initCameraPos;
    glm::vec3 cameraPositionTemp;
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::vec3 Front;
    glm::vec3 Right;
    glm::vec3 Up;

    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    float t = 0.0f, ambient = 0.7f;
    float Pitch = 0.0f, Roll = 0.0f, Yaw = 90.0f;
    float movementSpeed = 0.3f, currentSpeed = 0.0f, sensitivity = 0.04f;
    float dx = 0.0f;
    bool cheat = false, check = true;
    float checkE = true, checkR = true;
    bool play;

    float mazeWidth, mazeHeight, cubeThickness;
    std::vector<glm::vec3> verticalWallPosition;
    std::vector<glm::vec3> horizontalWallPosition;
    std::vector<std::pair<int, int>> path;

};
