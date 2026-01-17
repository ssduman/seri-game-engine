#pragma once

#include "seri/util/Util.h"
#include "seri/shader/ShaderBase.h"
#include "seri/shader/ShaderLibrary.h"

#include <glad/gl.h>

namespace seri
{
	class ShaderOpenGL : public ShaderBase
	{
	public:
		void Init(const std::string& shaderPath) override
		{
			std::string shaderText = Util::ReadFileAtPath(shaderPath.c_str());
			std::string vsCode = Util::GetContentOfToken(shaderText, "#beg_vs", "#end_vs");
			std::string fsCode = Util::GetContentOfToken(shaderText, "#beg_fs", "#end_fs");

			Build(vsCode, fsCode);
		}

		void Init(const std::string& vsCode, const std::string& fsCode) override
		{
			Build(vsCode, fsCode);
		}

		void Build(const std::string& vsCode, const std::string& fsCode) override
		{
			_vsCode = ParseIncludes(vsCode);
			_fsCode = ParseIncludes(fsCode);

			unsigned int vertexShader = CompileShader(GL_VERTEX_SHADER, _vsCode.c_str());
			unsigned int fragmentShader = CompileShader(GL_FRAGMENT_SHADER, _fsCode.c_str());

			_program = glCreateProgram();
			glAttachShader(_program, vertexShader);
			glAttachShader(_program, fragmentShader);
			glLinkProgram(_program);
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			CheckProgramLinkingError();
			ParseUniforms();
		}

		void ParseUniforms() override
		{
			if (_program == 0)
			{
				return;
			}

			int32_t currSlot = 0;

			_uniforms.clear();

			GLint uniformCount = 0;
			glGetProgramiv(_program, GL_ACTIVE_UNIFORMS, &uniformCount);

			GLint maxNameLength = 0;
			glGetProgramiv(_program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxNameLength);

			std::vector<char> nameBuffer(maxNameLength);

			for (GLint i = 0; i < uniformCount; i++)
			{
				GLsizei length = 0;
				GLint size = 0;
				GLenum type = 0;
				GLint blockIndex = -1;

				glGetActiveUniform(_program, i, maxNameLength, &length, &size, &type, nameBuffer.data());
				glGetActiveUniformsiv(_program, 1, (GLuint*)&i, GL_UNIFORM_BLOCK_INDEX, &blockIndex);

				if (blockIndex != -1)
				{
					continue;
				}

				std::string name(nameBuffer.data(), length);

				uint32_t location = glGetUniformLocation(_program, name.c_str());

				UniformType uniformType = GetUniformType(type);
				if (uniformType == UniformType::unknown_type)
				{
					continue;
				}

				int32_t slot = IsSamplerType(uniformType) ? currSlot++ : -1;

				_uniforms.push_back(
					{
						.name = name,
						.type = uniformType,
						.slot = slot,
						.location = location,
					}
					);
			}
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

		void SetBool(const std::string& name, bool value) override
		{
			glUniform1i(GetUniformLocation(name), value);
		}

		void SetInt(const std::string& name, int value) override
		{
			glUniform1i(GetUniformLocation(name), value);
		}

		void SetInt2(const std::string& name, const glm::ivec2& value) override
		{
			glUniform2iv(GetUniformLocation(name), 1, &value[0]);
		}

		void SetInt3(const std::string& name, const glm::ivec3& value) override
		{
			glUniform3iv(GetUniformLocation(name), 1, &value[0]);
		}

		void SetInt4(const std::string& name, const glm::ivec4& value) override
		{
			glUniform4iv(GetUniformLocation(name), 1, &value[0]);
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

		std::shared_ptr<ShaderBase> Clone() override
		{
			auto shader = ShaderBase::Create();
			shader->Init(_vsCode.c_str(), _fsCode.c_str());
			shader->id = id;
			return shader;
		}

	private:
		unsigned int CompileShader(GLenum type, const char* code)
		{
			unsigned int shader = glCreateShader(type);

			glShaderSource(shader, 1, &code, nullptr);
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

		int GetUniformLocation(const std::string& name)
		{
			return glGetUniformLocation(_program, name.c_str());
		}

		std::string ParseIncludes(const std::string& code)
		{
			int lineNumber = 0;
			std::string line;
			std::stringstream result;
			std::stringstream contentStream(code);
			while (std::getline(contentStream, line))
			{
				if (line.find("#include") != std::string::npos)
				{
					size_t beg = line.find('"');
					size_t end = line.find('"', beg + 1);

					if (beg != std::string::npos && end != std::string::npos)
					{
						std::string includeFile = line.substr(beg + 1, end - beg - 1);

						result << "// BEGIN INCLUDE: " << includeFile << "\n";
						result << seri::ShaderLibrary::GetGLSL(includeFile).code;
						result << "// END INCLUDE: " << includeFile << "\n";
						result << "#line " << lineNumber << "\n";
					}
				}
				else
				{
					result << line << "\n";
				}

				lineNumber += 1;
			}

			return result.str();
		}

		UniformType GetUniformType(GLint type)
		{
			switch (type)
			{
				case GL_BOOL: return UniformType::bool_type;

				case GL_FLOAT: return UniformType::float_type;
				case GL_FLOAT_VEC2: return UniformType::vec2_type;
				case GL_FLOAT_VEC3: return UniformType::vec3_type;
				case GL_FLOAT_VEC4: return UniformType::vec4_type;

				case GL_INT: return UniformType::int_type;
				case GL_INT_VEC2: return UniformType::ivec2_type;
				case GL_INT_VEC3: return UniformType::ivec3_type;
				case GL_INT_VEC4: return UniformType::ivec4_type;

				case GL_SAMPLER_CUBE: return UniformType::cubemap_type;
				case GL_SAMPLER_2D: return UniformType::sampler2d_type;

				default: return UniformType::unknown_type;
			}
		}

		bool IsSamplerType(UniformType type)
		{
			switch (type)
			{
				case seri::UniformType::cubemap_type:
				case seri::UniformType::sampler2d_type:
					return true;
				default:
					return false;
			}
		}

		unsigned int _program{ 0 };

	};
}
