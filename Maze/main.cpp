#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"

#include "Window.h"
#include "Maze.h"
#include "Camera.h"

#include <iostream>

/* --TODO--
1- restart game
2- add light
3- user input, maze size, sensitivity
4- win situation
5- only work same widht and height 
*/

double mouseCurrentPosX, mouseCurrentPosY;

Camera *camera;

void mouseMoveCallback(GLFWwindow *window, double mouseXPos, double mouseYPos) {
	double deltaX = mouseXPos - mouseCurrentPosX;
	double deltaY = mouseYPos - mouseCurrentPosY;

	camera->mouseControl(window, deltaX, deltaY);

	mouseCurrentPosX = mouseXPos;
	mouseCurrentPosY = mouseYPos;
}

int main(void) {
	Window thisWindow(true);
	GLFWwindow *window = thisWindow.getWindow();
	float aspect = ((float)thisWindow.getWidth()) / thisWindow.getHeight();
	mouseCurrentPosX = thisWindow.getMouseX();
	mouseCurrentPosY = thisWindow.getMouseY();

	newMaze *maze;
	int mazeWidth = 20;		// only works same width-height size
	int mazeHeight = 20;
	int cubeThickness = 5.0f;	// cubes will be 10.0f x 10.0f x 10.f
	maze = new newMaze(cubeThickness, mazeWidth, mazeHeight);

	glm::vec3 cameraPosition = glm::vec3((mazeWidth - 1) * cubeThickness * 2, -cubeThickness / 2, -cubeThickness * 4);
	camera = new Camera(cameraPosition, aspect);
	camera->setMaze(maze);
	camera->setMazeWidth(mazeWidth);
	camera->setMazeHeight(mazeHeight);
	camera->setCubeThickness(cubeThickness);
	camera->setVerticalWallPos(maze->getVerticalWallPosition());
	camera->setHorizontalWallPos(maze->getHorizontalWallPosition());

	glfwSetCursorPosCallback(window, mouseMoveCallback);

	int escaping = false;
	int check = true;
	while (!glfwWindowShouldClose(window)) {
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		glClearColor(0.4f, 0.4f, 0.4f, 1.0f);

		if ((glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) && (check)) {
			if (escaping == false) {
				escaping = true;
			}
			else {
				escaping = false;
			}
			check = false;
		}
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE) {
			check = true;
		}
		
		camera->keyboardControl(window);
		camera->update();

		if (escaping == false) {
			maze->display(false);
		}
		else {
			maze->display(true);
		}

		//maze->display();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
