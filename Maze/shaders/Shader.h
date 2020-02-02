#pragma once

#include <GL/glew.h>

#include "glm/glm.hpp"

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>

class Shader {
private:
	unsigned int ID = 0;
	std::string loadshader(const std::string &path);

public:
	Shader(const std::string &vs_path, const std::string &fs_path);
	~Shader();
	void bind();
	void unbind();
	void setInt(const std::string &name, int value) const {
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}
	void setFloat(const std::string &name, float value) const {
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}
	void setVec3(const std::string &name, const glm::vec3 &value) const {
		glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void setVec3(const std::string &name, float x, float y, float z) const {
		glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
	}
	void setMat4(const std::string &name, const glm::mat4 &mat) const {
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
};
