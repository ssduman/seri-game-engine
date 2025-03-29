#pragma once

#include "seri/logging/Logger.h"

#include <glad/gl.h>
#include <glm/glm.hpp>

#include <string>

class Shader
{
public:
	Shader() = default;

	Shader(Shader& other) = default;

	Shader(Shader&& other) = default;

	Shader& operator=(Shader& other) = default;

	Shader& operator=(Shader&& other) = default;

	~Shader() = default;

	void init(const char* vsCodePath, const char* fsCodePath, bool readFromFile = true)
	{
		unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vsCodePath, readFromFile);
		unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fsCodePath, readFromFile);

		_program = glCreateProgram();
		glAttachShader(_program, vertexShader);
		glAttachShader(_program, fragmentShader);
		glLinkProgram(_program);
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		checkProgramLinkingError();
	}

	void use()
	{
		if (!isActiveForUsing())
		{
			LOGGER(error, "shader is not active");
			return;
		}

		glUseProgram(_program);
	}

	void disuse()
	{
		glUseProgram(0);
	}

	void release()
	{
		if (_program != 0)
		{
			LOGGER(verbose, "deleted shader: " << _program);
			disuse();
			del();
		}
	}

	unsigned int getProgram()
	{
		return _program;
	}

	const unsigned int getProgram() const
	{
		return _program;
	}

private:
	unsigned int compileShader(GLenum type, const char* code, bool readFromFile)
	{
		unsigned int shader = glCreateShader(type);
		auto codeCStr = code;
		std::string codeStr;
		if (readFromFile)
		{
			codeStr = Util::readFileAtPath(code);
			codeCStr = codeStr.c_str();
		}
		glShaderSource(shader, 1, &codeCStr, nullptr);
		glCompileShader(shader);
		checkShaderCompilationError(shader);
		return shader;
	}

	bool checkShaderCompilationError(unsigned int shader)
	{
		int errorStatusSuccess;
		char errorStatusLog[512];
		glGetShaderiv(shader, GL_COMPILE_STATUS, &errorStatusSuccess);
		if (!errorStatusSuccess)
		{
			glGetShaderInfoLog(shader, 512, nullptr, errorStatusLog);
			LOGGER(error, "shader compilation failed: " << errorStatusLog);
			return false;
		}

		return true;
	}

	bool checkProgramLinkingError()
	{
		int errorStatusSuccess;
		char errorStatusLog[512];
		glGetProgramiv(_program, GL_LINK_STATUS, &errorStatusSuccess);
		if (!errorStatusSuccess)
		{
			glGetShaderInfoLog(_program, 512, nullptr, errorStatusLog);
			LOGGER(error, "program linking failed: " << errorStatusLog);
			return false;
		}

		return true;
	}

	bool isActiveForUsing()
	{
		return _program != 0;
	}

	void del()
	{
		glDeleteProgram(_program);
		_program = 0;
	}

	unsigned int _program{ 0 };

};
