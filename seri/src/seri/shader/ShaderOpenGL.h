#pragma once

#include "seri/util/Util.h"
#include "seri/shader/ShaderBase.h"

#include <glad/gl.h>

#include <string>

namespace seri
{
	class ShaderOpenGL : public ShaderBase
	{
	public:
		void Init(const char* vsCodePath, const char* fsCodePath, bool readFromFile = true) override
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

		void Bind() override
		{
			if (!IsActiveForUsing())
			{
				LOGGER(error, "[shader] not active for bind");
				return;
			}

			glUseProgram(_program);
		}

		void Unbind() override
		{
			glUseProgram(0);
		}

		void Release() override
		{
			if (_program != 0)
			{
				Unbind();
				Del();
				LOGGER(verbose, "[shader] deleted: " << _program);
			}
		}

		bool IsActiveForUsing() override
		{
			return _program != 0;
		}

		void SetInt(const std::string& name, int value) override
		{
			glUniform1i(GetUniformLocation(name), value);
		}

		void SetBool(const std::string& name, bool value) override
		{
			glUniform1i(GetUniformLocation(name), value);
		}

		void SetFloat(const std::string& name, float value) override
		{
			glUniform1f(GetUniformLocation(name), value);
		}

		void SetFloat2(const std::string& name, const glm::vec2& value) override
		{
			glUniform2fv(GetUniformLocation(name), 1, &value[0]);
		}

		void SetFloat3(const std::string& name, const glm::vec3& value) override
		{
			glUniform3fv(GetUniformLocation(name), 1, &value[0]);
		}

		void SetFloat4(const std::string& name, const glm::vec4& value) override
		{
			glUniform4fv(GetUniformLocation(name), 1, &value[0]);
		}

		void SetMat4(const std::string& name, const glm::mat4& value) override
		{
			glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &value[0][0]);
		}

		int GetUniformLocation(const std::string& name)
		{
			return glGetUniformLocation(_program, name.c_str());
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
				LOGGER(error, "[shader] program compilation failed: " << errorStatusLog);
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
				LOGGER(error, "[shader] program linking failed: " << errorStatusLog);
				return false;
			}

			return true;
		}

		void Del()
		{
			glDeleteProgram(_program);
			_program = 0;
		}

		unsigned int _program{ 0 };

	};
}
