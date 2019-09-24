#include "Camera.h"

Camera::Camera(glm::vec3 positionCamera, float aspect) {
	shader = new Shader("shaders/basic_vs.shader", "shaders/basic_fs.shader");
	shader->bind();

	perspectiveMatrix = glm::perspective(30.0f, aspect, 0.1f, 1000.0f);

	cameraPosition = positionCamera;
	cameraPositionTemp = cameraPosition;
	Up = cameraUp;

	updateVectors();

	view = glm::lookAt(cameraPosition, cameraPosition + Front, Up);
	mvp = glm::mat4(perspectiveMatrix * view);

	update();
}

glm::vec3 Camera::projectionVector(glm::vec3 Front, glm::vec3 Right) {
	glm::vec3 u = glm::cross(cameraUp, Right);
	glm::vec3 v = Front;

	return ((u * v) / pow(glm::length(u), 2)) * u;
}

void Camera::keyboardControl(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	//if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
	//	escape();
	//}

	if ((glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) && (check)) {
		cheat = (cheat == false) ? true : false;
		check = false;
	}
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE) {
		check = true;
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		cameraPositionTemp += glm::normalize(projectionVector(Front, Right)) * movementSpeed;
		if ((canIPass(cameraPositionTemp) == false) && (!cheat)) {
			cameraPositionTemp = cameraPosition;
		}
		else if (canIPass(cameraPosition) && (!cheat)) {
			cameraPosition += glm::normalize(projectionVector(Front, Right)) * movementSpeed;
			cameraPositionTemp = cameraPosition;
		}
		else if (cheat) {
			cameraPosition += Front * movementSpeed;
			cameraPositionTemp = cameraPosition;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		cameraPositionTemp -= glm::normalize(projectionVector(Front, Right)) * movementSpeed;
		if ((canIPass(cameraPositionTemp) == false) && (!cheat)) {
			cameraPositionTemp = cameraPosition;
		}
		else if (canIPass(cameraPosition) && (!cheat)) {
			cameraPosition -= glm::normalize(projectionVector(Front, Right)) * movementSpeed;
			cameraPositionTemp = cameraPosition;
		}
		else if (cheat) {
			cameraPosition -= Front * movementSpeed;
			cameraPositionTemp = cameraPosition;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		cameraPositionTemp -= Right * movementSpeed;
		if ((canIPass(cameraPositionTemp) == false) && (!cheat)) {
			cameraPositionTemp = cameraPosition;
		}
		else if ((canIPass(cameraPosition)) && (!cheat)) {
			cameraPosition -= Right * (movementSpeed);
			cameraPositionTemp = cameraPosition;
		}
		else if (cheat) {
			cameraPosition -= Right * (movementSpeed);
			cameraPositionTemp = cameraPosition;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		cameraPositionTemp += Right * movementSpeed;
		if ((canIPass(cameraPositionTemp) == false) && (!cheat)) {
			cameraPositionTemp = cameraPosition;
		}
		else if ((canIPass(cameraPosition)) && (!cheat)) {
			cameraPosition += Right * (movementSpeed);
			cameraPositionTemp = cameraPosition;
		}
		else if (cheat) {
			cameraPosition += Right * (movementSpeed);
			cameraPositionTemp = cameraPosition;
		}
	}

	view = glm::lookAt(cameraPosition, cameraPosition + Front, Up);
}

bool Camera::canIPass(glm::vec3 position) {
	float currentX = position.x;
	float currentZ = position.z;

	int currentXPosAtMaze = (currentX - (-cubeThickness)) / (cubeThickness * 2);
	int currentYPosAtMaze = (currentZ - (-cubeThickness)) / (cubeThickness * 2);

	bool cond1 = currentZ > -cubeThickness;	// down bound
	bool cond2 = currentZ < cubeThickness * 2 * mazeHeight - cubeThickness;	// upper bound
	bool cond3 = currentX < cubeThickness * 2 * mazeWidth - cubeThickness;	// right bound
	bool cond4 = currentXPosAtMaze >= 0 && currentXPosAtMaze < mazeWidth;	// legal index
	bool cond5 = currentYPosAtMaze >= 0 && currentYPosAtMaze < mazeHeight;	// legal index
	
	if (cond1 && cond2 && cond3 & cond4 && cond5) {
		std::vector<glm::vec3>::iterator it1;
		std::vector<glm::vec3>::iterator it2;

		position.x = std::round(position.x);
		position.y = 0.0f;
		position.z = std::round(position.z);

		glm::vec3 vertical(position.x, 0.0f, (std::round(position.z / 10)) * 10);
		glm::vec3 horizontal((std::round(position.x / 10)) * 10, 0.0f, position.z);

		it1 = std::find(verticalWallPosition.begin(), verticalWallPosition.end(), vertical);
		it2 = std::find(horizontalWallPosition.begin(), horizontalWallPosition.end(), horizontal);
		if ((it1 == verticalWallPosition.end()) && (it2 == horizontalWallPosition.end())) {
			return true;
		}
	}
	else {
		return true;
	}

	return false;
}

void Camera::mouseControl(GLFWwindow *window, double deltaX, double deltaY) {
	Yaw += deltaX * sensitivity;
	Pitch += deltaY * sensitivity;

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

void Camera::updateVectors() {
	glm::vec3 eulerAngle;
	eulerAngle.x = -cos(glm::radians(Pitch)) * cos(glm::radians(Yaw));
	eulerAngle.y = sin(glm::radians(Pitch));
	eulerAngle.z = cos(glm::radians(Pitch)) * sin(glm::radians(Yaw));

	Front = glm::normalize(eulerAngle);
	Right = glm::normalize(glm::cross(Front, cameraUp));
	Up = glm::normalize(glm::cross(Right, Front));
}

glm::mat4 Camera::getMVP() {
	return glm::mat4(perspectiveMatrix * view);
}

void Camera::update() {
	shader->setUmat4f("mvp", getMVP());
}
