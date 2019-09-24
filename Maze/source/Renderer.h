#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"

#include <iostream>
#include <vector>

class Renderer {
private:
	unsigned int va, count = 0;
	unsigned int indices[36] = {
		0,   1,  2,  0,  2,  3,
		4,   5,  6,  4,  6,  7,
		8,   9, 10,  8, 10, 11,
		12, 13, 14, 12, 14, 15,
		16, 17, 18, 16, 18, 19,
		20, 22, 21, 20, 23, 22
	};

public:
	Renderer(float w, float h, float z);
	void init(unsigned int c, std::vector<glm::vec3> &coordinates);
	void render();
};


