#include "Shader.h"

Shader::Shader(const std::string &vs_path, const std::string &fs_path) {
	int vs, fs;
	vs = glCreateShader(GL_VERTEX_SHADER);
	std::string vs_source = loadshader(vs_path);
	const char *vs_src = vs_source.c_str();
	glShaderSource(vs, 1, &vs_src, NULL);
	glCompileShader(vs);

	fs = glCreateShader(GL_FRAGMENT_SHADER);
	std::string fs_source = loadshader(fs_path);
	const char *fs_src = fs_source.c_str();
	glShaderSource(fs, 1, &fs_src, NULL);
	glCompileShader(fs);

	ID = glCreateProgram();
	glAttachShader(ID, vs);
	glAttachShader(ID, fs);
	glLinkProgram(ID);

	glDeleteShader(vs);
	glDeleteShader(fs);
}

Shader::~Shader() {
	glDeleteProgram(ID);
}

void Shader::bind() {
	glUseProgram(ID);
}

void Shader::unbind() {
	glUseProgram(0);
}

std::string Shader::loadshader(const std::string &path) {
	std::ifstream stream(path);
	std::string line;
	std::stringstream ss;

	while (getline(stream, line)) {
		ss << line << "\n";
	}

	return ss.str();
}
