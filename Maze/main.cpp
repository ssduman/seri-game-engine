﻿#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Window.h"
#include "Maze.h"
#include "Camera.h"
#include "Skybox.h"
#include "Light.h"

/* --TODO--
1- user input-> maze size, sensitivity...
*/

double mouseCurrentPosX, mouseCurrentPosY;

Camera *camera;
glm::vec3 cameraPosition;

Maze *maze;
int mazeWidth = 50;
int mazeHeight = 50;
int thickness = 5.0f;	// cubes will be 10.0f x 10.0f x 10.f

Light *light;

void mouseMoveCallback(GLFWwindow *window, double mouseXPos, double mouseYPos) {
	double deltaX = mouseXPos - mouseCurrentPosX;
	double deltaY = mouseYPos - mouseCurrentPosY;

	camera->mouseControl(window, deltaX, deltaY);

	mouseCurrentPosX = mouseXPos;
	mouseCurrentPosY = mouseYPos;
}

void mouseScrollCallback(GLFWwindow *window, double x, double y) {
	float curr = camera->getAmbient();
	if ((y > 0) && (curr <= 0.98f)) {
		camera->adjustAmbient(0.02f);
	}
	else if ((y < 0) && (curr >= 0.02f)) {
		camera->adjustAmbient(-0.02f);
	}
}

void win(float &r, float &g, float &b) {
	glm::vec3 pos = camera->getCameraPos();
	int t1 = thickness, t2 = thickness * 2, h = mazeHeight;
	bool x = pos.x >= -thickness && pos.x <= thickness;
	bool z = pos.z >= (h - 2) * t2 + t1 && pos.z <= (h - 1) * t2 + t1;
	if (x && z) { r = 0.2f; g = 0.8f; b = 0.4f; }
	else { r = 0.4f; g = 0.4f; b = 0.4f; }
}

void keyboardControl(GLFWwindow *window, bool &escaping, bool &restart, bool &checkE, bool &checkR) {
	if ((glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) && (checkE)) {
		escaping = escaping == false ? true : false;
		checkE = false;
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE) {
		checkE = true;
	}

	if ((glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) && (checkR)) {
		camera->setCameraPos(cameraPosition);
		delete maze;
		maze = new Maze(thickness, mazeWidth, mazeHeight);
		camera->setWallPos(maze->getVerticalWallPosition(), maze->getHorizontalWallPosition());
		checkR = false;
		escaping = false;
	}
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE) {
		checkR = true;
	}
}

int main(void) {
	Window thisWindow(true);
	GLFWwindow *window = thisWindow.getWindow();
	float aspect = ((float)thisWindow.getWidth()) / thisWindow.getHeight();
	mouseCurrentPosX = thisWindow.getMouseX();
	mouseCurrentPosY = thisWindow.getMouseY();
	glfwSetCursorPosCallback(window, mouseMoveCallback);
	glfwSetScrollCallback(window, mouseScrollCallback);

	maze = new Maze(thickness, mazeWidth, mazeHeight);

	// camera looks towards maze enterance
	cameraPosition = glm::vec3((mazeWidth - 1) * thickness * 2, -thickness / 2, -thickness * 4);
	camera = new Camera(cameraPosition, aspect);
	camera->setDimensions(mazeWidth, mazeHeight, thickness);
	camera->setWallPos(maze->getVerticalWallPosition(), maze->getHorizontalWallPosition());

	light = new Light();
	Skybox *skybox = new Skybox();

	bool escaping = false;	// shows escape path
	bool checkE = true;		// helper for escaping variable
	bool restart = false;
	bool checkR = true;		// helper for restart variable
	float r = 0.4f, g = 0.4f, b = 0.4f;
	while (!glfwWindowShouldClose(window)) {
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		glClearColor(r, g, b, 1.0f);

		win(r, g, b);
		keyboardControl(window, escaping, restart, checkE, checkR);

		camera->keyboardControl(window);
		camera->update();

		maze->display(escaping);

		glm::mat4 view = camera->getView();
		glm::mat4 projection = camera->getProjection();
		skybox->display(view, projection);
		light->light(cameraPosition + glm::vec3(0, -10, 20), view, projection);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
