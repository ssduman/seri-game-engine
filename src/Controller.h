#pragma once

#include "Game.h"
#include "Camera.h"
#include "Window.h"
#include "IControler.h"

#include <string>
#include <vector>
#include <sstream>

class Controler : public IControler
{
public:
    Controler(Window window) :
        IControler(window) {
    };

    ~Controler() = default;

    void registerCamera(Camera* camera) override {
        _camera = camera;
    }

    void registerGame(Game* game) override {
        _game = game;
    }

    void mouseMoveCallback(GLFWwindow* window, double mouseXPos, double mouseYPos) override {
        double deltaX = mouseXPos - mouseCurrentPosX;
        double deltaY = mouseYPos - mouseCurrentPosY;

        _camera->mouseControl(window, deltaX, deltaY);

        mouseCurrentPosX = mouseXPos;
        mouseCurrentPosY = mouseYPos;
    }

    void mouseScrollCallback(GLFWwindow* window, double x, double y) override {
        float curr = _camera->getAmbient();
        if ((y > 0) && (curr <= 0.98f)) {
            _camera->adjustAmbient(0.02f);
        }
        else if ((y < 0) && (curr >= 0.02f)) {
            _camera->adjustAmbient(-0.02f);
        }
    }

    void charCallback(GLFWwindow* window, unsigned int c) override {
        userInput += c;
    }

    void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) override {
        if ((key == GLFW_KEY_ENTER) && (action == GLFW_PRESS)) {
            if (userInput.size() == 0) {
                return;
            }
            std::stringstream userInputStream(userInput);
            std::string part;
            std::vector<std::string> parts;

            while (std::getline(userInputStream, part, 'x')) {
                parts.push_back(part);
            }

            if (userInput == part || parts[0].size() == 0 || parts.size() != 2) {
                return;
            }

            std::stringstream width_s(parts[0]);
            std::stringstream height_s(parts[1]);
            //width_s >> mazeWidth;
            //height_s >> mazeHeight;
            //if ((mazeWidth <= 0) || (mazeHeight <= 0)) {
            //    return;
            //}

            //play = true;

            //delete maze, camera, game;

            //maze = new Maze(thickness, mazeWidth, mazeHeight);

            //auto cameraPosition = glm::vec3((mazeWidth - 1) * thickness * 2, -thickness / 2, -thickness * 4); // enterence
            //_camera = new Camera(cameraPosition, ((float)width) / height, play);
            //_camera->setDimensions(mazeWidth, mazeHeight, thickness);
            //_camera->setWallPos(maze->getVerticalWallPosition(), maze->getHorizontalWallPosition());

            userInput = "";
        }
        else if ((key == GLFW_KEY_BACKSPACE) && (action == GLFW_PRESS)) {
            if (userInput.size() > 0) {
                userInput.pop_back();
            }
        }
    }

private:
    //Game _game;
    //Camera _camera;
    //Window _window;
    //std::string userInput;
    //double mouseCurrentPosX;
    //double mouseCurrentPosY;
};
