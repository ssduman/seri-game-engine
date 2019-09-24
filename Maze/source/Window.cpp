#include "Window.h"

void mouseCallback(GLFWwindow *window, int button, int action, int mods) {
	if ((button == GLFW_MOUSE_BUTTON_LEFT) && (action == GLFW_PRESS)) {
	}
}

Window::Window(bool fullscreen) {
	if (!glfwInit()) {
		std::cout << "glfwInit error" << std::endl;
		exit(-1);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	int monXPos, monYPos;
	int widthTemp, heightTemp;
	glfwGetMonitorWorkarea(glfwGetPrimaryMonitor(), &monXPos, &monYPos, &widthTemp, &heightTemp);
	if (fullscreen) {
		this->window = glfwCreateWindow(widthTemp, heightTemp, "Maze", glfwGetPrimaryMonitor(), NULL);
		this->width = widthTemp;
		this->height = heightTemp;
	}
	else {
		this->window = glfwCreateWindow(1200, 1000, "Maze", NULL, NULL);
		this->width = 1200;
		this->height = 1000;
	}

	if (!window) {
		glfwTerminate();
		std::cout << "window error" << std::endl;
		exit(-1);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK) {
		std::cout << "glewInit Error" << std::endl;
		exit(-1);
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	setCursorPosMiddle();
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwSetMouseButtonCallback(window, mouseCallback);
}

int Window::getWidth() {
	return this->width;
}

int Window::getHeight() {
	return this->height;
}

double Window::getMouseX() {
	glfwGetCursorPos(window, &mouseX, &mouseY);
	return mouseX;
}

double Window::getMouseY() {
	glfwGetCursorPos(window, &mouseX, &mouseY);
	return mouseY;
}

void Window::setCursorPosMiddle() {
	glfwSetCursorPos(this->window, width / 2, height / 2);
}

GLFWwindow *Window::getWindow() {
	return this->window;
}