#pragma once

#include "seri/logging/Logger.h"
#include "seri/renderer/AuxiliaryStructs.h"

#include <glm/glm.hpp>

#include <string>

namespace seri
{
	class Shader
	{
	public:
		Shader() = default;

		Shader(Shader& other) = default;

		Shader(Shader&& other) = default;

		Shader& operator=(Shader& other) = default;

		Shader& operator=(Shader&& other) = default;

		~Shader() = default;

		void Init(const char* vsCodePath, const char* fsCodePath, bool readFromFile = true)
		{
			unsigned int vertexShader = CompileShader(GL_VERTEX_SHADER, vsCodePath, readFromFile);
			unsigned int fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fsCodePath, readFromFile);

			_program = glCreateProgram();
			glAttachShader(_program, vertexShader);
			glAttachShader(_program, fragmentShader);
			glLinkProgram(_program);
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			CheckProgramLinkingError();
		}

		void Use()
		{
			if (!IsActiveForUsing())
			{
				LOGGER(error, "shader is not active");
				return;
			}

			glUseProgram(_program);
		}

		void Disuse()
		{
			glUseProgram(0);
		}

		void Release()
		{
			if (_program != 0)
			{
				LOGGER(verbose, "deleted shader: " << _program);
				Disuse();
				Del();
			}
		}

		unsigned int GetProgram()
		{
			return _program;
		}

		const unsigned int GetProgram() const
		{
			return _program;
		}

	private:
		unsigned int CompileShader(GLenum type, const char* code, bool readFromFile)
		{
			unsigned int shader = glCreateShader(type);
			auto codeCStr = code;
			std::string codeStr;
			if (readFromFile)
			{
				codeStr = Util::ReadFileAtPath(code);
				codeCStr = codeStr.c_str();
			}
			glShaderSource(shader, 1, &codeCStr, nullptr);
			glCompileShader(shader);
			CheckShaderCompilationError(shader);
			return shader;
		}

		bool CheckShaderCompilationError(unsigned int shader)
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

		bool CheckProgramLinkingError()
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

		bool IsActiveForUsing()
		{
			return _program != 0;
		}

		void Del()
		{
			glDeleteProgram(_program);
			_program = 0;
		}

		unsigned int _program{ 0 };

	};
}
