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
	initView = view;
	mvp = glm::mat4(perspectiveMatrix * view);

	update();
}

void Camera::setCameraPos(glm::vec3 pos) {
	cameraPosition = pos;
	view = initView;
	Pitch = 0.0f; Roll = 0.0f; Yaw = 90.0f;
	updateVectors();
	update();
}

glm::vec3 Camera::projectionVector(glm::vec3 Front, glm::vec3 Right) {
	// using for walking same speed for any angles
	glm::vec3 u = glm::cross(cameraUp, Right);
	glm::vec3 v = Front;

	return ((u * v) / pow(glm::length(u), 2)) * u;
}

void Camera::keyboardControl(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
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

	// coordinat based location to index based location
	int currentXPosAtMaze = (currentX - (-cubeThickness)) / (cubeThickness * 2);
	int currentYPosAtMaze = (currentZ - (-cubeThickness)) / (cubeThickness * 2);

	bool cond1 = currentZ > -cubeThickness;	// lower bound
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

		/*			A 5x5 maze
		 +Z
		  ^
		  |
		  |		o  o--o--o--o--o
		  |		|              |
		  |		o  o--o--o--o  o
		  |		|  |        |  |
		  |		o  o--o--o  o  o
		  |		|        |     |
		  |		o  o--o  o--o--o
		  |		|  |  |        |
		  |		o  o  o--o--o  o
		  |		|     |        |
		  |		o--o--o--o--o  o
		  |				  (camera)
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
