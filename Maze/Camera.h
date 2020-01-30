#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "shaders/Shader.h"

#include <iostream>
#include <cmath>
#include <vector>

#include "Maze.h"

class Camera {
private:
	Shader *shader;

	glm::mat4 perspectiveMatrix;
	glm::mat4 view, initView;
	glm::mat4 mvp;

	glm::vec3 cameraPosition;
	glm::vec3 cameraPositionTemp;
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	glm::vec3 Front;
	glm::vec3 Right;
	glm::vec3 Up;

	float t = 0.0f;
	float Pitch = 0.0f, Roll = 0.0f, Yaw = 90.0f;
	float movementSpeed = 0.3f, currentSpeed = 0.0f, sensitivity = 0.04f;
	bool cheat = false;
	bool check = true;

	Maze *maze;
	int mazeWidth, mazeHeight;
	float cubeThickness;
	std::vector<glm::vec3> verticalWallPosition;
	std::vector<glm::vec3> horizontalWallPosition;
	std::vector<std::pair<int, int>> path;

	void updateVectors();
	bool canIPass(glm::vec3 position);
	glm::vec3 projectionVector(glm::vec3 Front, glm::vec3 Right);

public:
	Camera(glm::vec3 positionCamera, float aspect);
	void mouseControl(GLFWwindow *window, double deltaX, double deltaY);
	void keyboardControl(GLFWwindow *window);
	void update();
	void setCameraPos(glm::vec3 pos);
	glm::vec3 getCameraPos() { return cameraPosition; }
	void setDimensions(int w, int h, int t) {
		mazeWidth = w; mazeHeight = h; cubeThickness = t;
	}
	void setWallPos(std::vector<glm::vec3> vp, std::vector<glm::vec3> hp) {
		verticalWallPosition = vp; horizontalWallPosition = hp;
	}
	glm::mat4 getMVP();
};
