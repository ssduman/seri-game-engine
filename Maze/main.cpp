#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"

#include "Window.h"
#include "Maze.h"
#include "Camera.h"
#include "Skybox.h"
#include "Light.h"
#include "Game.h"

double mouseCurrentPosX, mouseCurrentPosY;

Camera *camera;
glm::vec3 cameraPosition;

Maze *maze;
float width, height, aspect;
int mazeWidth = 20, mazeHeight = 20, thickness = 5.0f;	// cubes will be 10.0f x 10.0f x 10.0f

Light *light;
Game *game;
std::string userInput = "";
bool play = false;

void mouseMoveCallback(GLFWwindow *window, double mouseXPos, double mouseYPos);
void mouseScrollCallback(GLFWwindow *window, double x, double y);
void charCallback(GLFWwindow *window, unsigned int c);
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
bool win();

int main(void) {
	Window thisWindow(true);	// play fullscreen for full functionality
	GLFWwindow *window = thisWindow.getWindow();

	width = (float)thisWindow.getWidth();
	height = (float)thisWindow.getHeight();

	mouseCurrentPosX = thisWindow.getMouseX(); 
	mouseCurrentPosY = thisWindow.getMouseY();

	glfwSetCursorPosCallback(window, mouseMoveCallback);
	glfwSetScrollCallback(window, mouseScrollCallback);
	glfwSetCharCallback(window, charCallback);
	glfwSetKeyCallback(window, keyCallback);

	maze = new Maze(thickness, mazeWidth, mazeHeight);

	cameraPosition = glm::vec3(0, -thickness * 5, -thickness * 4 - mazeHeight * thickness);
	camera = new Camera(cameraPosition, ((float)width) / height, play);
	camera->setDimensions(mazeWidth, mazeHeight, thickness);
	camera->setWallPos(maze->getVerticalWallPosition(), maze->getHorizontalWallPosition());

	Skybox *skybox = new Skybox();
	light = new Light();
	game = new Game(width, height);

	bool escaping = false, restart = false, inExit = false;
	while (!glfwWindowShouldClose(window)) {
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		glClearColor(0.4f, 0.4f, 0.4f, 1.0f);

		camera->keyboardControl(window, &maze, play, escaping, restart);
		camera->update(play);

		maze->display(escaping);

		glm::mat4 view = camera->getView();
		glm::mat4 projection = camera->getProjection();
		skybox->display(view, projection);
		light->light(cameraPosition, view, projection);

		game->display(userInput, play, restart, win());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

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

void charCallback(GLFWwindow *window, unsigned int c) {
	userInput += c;
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
	if ((key == GLFW_KEY_ENTER) && (action == GLFW_PRESS)) {
		if (userInput.size() == 0) return;
		std::stringstream userInputStream(userInput);
		std::string part;
		std::vector<std::string> parts;

		while (std::getline(userInputStream, part, 'x')) {
			parts.push_back(part);
		}
		if (userInput == part) return;
		if (parts[0].size() == 0) return;
		if (parts.size() != 2) return;

		std::stringstream width_s(parts[0]);
		std::stringstream height_s(parts[1]);
		width_s >> mazeWidth;
		height_s >> mazeHeight;
		if ((mazeWidth == 0) || (mazeHeight == 0)) return;

		play = true;

		delete maze, camera, game;

		maze = new Maze(thickness, mazeWidth, mazeHeight);

		cameraPosition = glm::vec3((mazeWidth - 1) * thickness * 2, -thickness / 2, -thickness * 4);	// enterence
		camera = new Camera(cameraPosition, ((float)width) / height, play);
		camera->setDimensions(mazeWidth, mazeHeight, thickness);
		camera->setWallPos(maze->getVerticalWallPosition(), maze->getHorizontalWallPosition());
	}
	else if ((key == GLFW_KEY_BACKSPACE) && (action == GLFW_PRESS)) {
		if (userInput.size() > 0) {
			userInput.pop_back();
		}
	}
}

bool win() {
	glm::vec3 pos = camera->getCameraPos();
	int t1 = thickness, t2 = thickness * 2, h = mazeHeight;
	bool x = pos.x >= -thickness && pos.x <= thickness;
	bool z = pos.z >= (h - 2) * t2 + t1 && pos.z <= (h - 1) * t2 + t1;
	if (x && z) return true;
	return false;
}
