#pragma once

#include "seri/shader/Shader.h"
#include "seri/camera/ICamera.h"

#include <memory>
#include <vector>
#include <filesystem>

class ShaderManager
{
public:
	ShaderManager(ShaderManager const&) = delete;

	void operator=(ShaderManager const&) = delete;

	static void Init(const char* shaderFolderPath)
	{
		GetInstance()._predefinedShaders = {};

		for (const auto& entry : std::filesystem::directory_iterator(shaderFolderPath))
		{
			std::string name = entry.path().stem().string();
			std::string text = Util::ReadFileAtPath(entry.path().string().c_str());

			ShaderInfo info;
			info.name = name;
			info.vsCode = Util::GetContentOfToken(text, "#beg_vs", "#end_vs");
			info.fsCode = Util::GetContentOfToken(text, "#beg_fs", "#end_fs");
			GetInstance()._predefinedShaders.push_back(info);
		}

		//LOGGER(info, "shader manager init done");
	}

	static ShaderManager& GetInstance()
	{
		static ShaderManager instance;
		return instance;
	}

	static std::shared_ptr<Shader> Find(std::string name)
	{
		for (auto& predef : GetInstance()._predefinedShaders)
		{
			if (predef.name == name)
			{
				auto shader = std::make_shared<Shader>();
				shader->init(predef.vsCode.c_str(), predef.fsCode.c_str(), /*readFromFile*/ false);
				return shader;
			}
		}
		LOGGER(error, "shader not found: " << name);
		return nullptr;
	}

	static void setTRS(Shader& shader, const std::shared_ptr<ICamera>& camera)
	{
		ShaderManager::Use(shader);
		ShaderManager::SetMat4(shader, "u_model", glm::mat4{ 1.0f });
		ShaderManager::SetMat4(shader, "u_view", camera->getView());
		ShaderManager::SetMat4(shader, "u_projection", camera->getProjection());
		ShaderManager::Disuse();
	}

	static void initMVP(Shader& shader, const std::shared_ptr<ICamera>& camera)
	{
		ShaderManager::Use(shader);
		ShaderManager::SetMat4(shader, "u_model", glm::mat4{ 1.0f });
		ShaderManager::SetMat4(shader, "u_view", camera->getView());
		ShaderManager::SetMat4(shader, "u_projection", camera->getProjection());
		ShaderManager::Disuse();
	}

	static void initVP(Shader& shader, const std::shared_ptr<ICamera>& camera)
	{
		ShaderManager::Use(shader);
		ShaderManager::SetMat4(shader, "u_view", camera->getView());
		ShaderManager::SetMat4(shader, "u_projection", camera->getProjection());
		ShaderManager::Disuse();
	}

	static void setModel(Shader& shader, const glm::mat4& model)
	{
		ShaderManager::Use(shader);
		ShaderManager::SetMat4(shader, "u_model", model);
		ShaderManager::Disuse();
	}

	static void setView(Shader& shader, const glm::mat4& view)
	{
		ShaderManager::Use(shader);
		ShaderManager::SetMat4(shader, "u_view", view);
		ShaderManager::Disuse();
	}

	static void setProjection(Shader& shader, const glm::mat4& projection)
	{
		ShaderManager::Use(shader);
		ShaderManager::SetMat4(shader, "u_projection", projection);
		ShaderManager::Disuse();
	}

	static void setPosition(Shader& shader, const glm::vec2& position)
	{
		ShaderManager::Use(shader);
		ShaderManager::SetVec2(shader, "u_position", position);
		ShaderManager::Disuse();
	}

	static void setPosition(Shader& shader, const glm::vec3& position)
	{
		ShaderManager::Use(shader);
		ShaderManager::SetVec3(shader, "u_position", position);
		ShaderManager::Disuse();
	}

	static void SetColor(Shader& shader, const glm::vec4& color)
	{
		ShaderManager::Use(shader);
		ShaderManager::SetVec4(shader, "u_color", color);
		ShaderManager::Disuse();
	}

	static void SetColor(const std::shared_ptr<Shader>& shader, const char* name, const glm::vec4& color)
	{
		glUniform4fv(ShaderManager::GetUniformLocation(shader, name), 1, &color[0]);
	}

	static void Use(Shader& shader)
	{
		shader.use();
	}

	static void Use(const std::shared_ptr<Shader>& shader)
	{
		shader->use();
	}

	static void Disuse()
	{
		glUseProgram(0);
	}

	static void Disuse(Shader& shader)
	{
		shader.disuse();
	}

	static int GetUniformLocation(const Shader& shader, const char* name)
	{
		return glGetUniformLocation(shader.getProgram(), name);
	}

	static int GetUniformLocation(const std::shared_ptr<Shader>& shader, const char* name)
	{
		return glGetUniformLocation(shader->getProgram(), name);
	}

	static void SetBool(const Shader& shader, const char* name, bool val)
	{
		glUniform1i(ShaderManager::GetUniformLocation(shader, name), val);
	}

	static void SetInt(const Shader& shader, const char* name, int val)
	{
		glUniform1i(ShaderManager::GetUniformLocation(shader, name), val);
	}

	static void SetUInt(const std::shared_ptr<Shader>& shader, const char* name, unsigned int val)
	{
		glUniform1i(ShaderManager::GetUniformLocation(shader, name), val);
	}

	static void SetFloat(const Shader& shader, const char* name, float val)
	{
		glUniform1f(ShaderManager::GetUniformLocation(shader, name), val);
	}

	static void SetVec2(const Shader& shader, const char* name, glm::vec2 val)
	{
		glUniform2fv(ShaderManager::GetUniformLocation(shader, name), 1, &val[0]);
	}

	static void SetVec3(const Shader& shader, const char* name, const glm::vec3& val)
	{
		glUniform3fv(ShaderManager::GetUniformLocation(shader, name), 1, &val[0]);
	}

	static void SetVec4(const Shader& shader, const char* name, const glm::vec4& val)
	{
		glUniform4fv(ShaderManager::GetUniformLocation(shader, name), 1, &val[0]);
	}

	static void SetVec4(const std::shared_ptr<Shader>& shader, const char* name, const glm::vec4& val)
	{
		glUniform4fv(ShaderManager::GetUniformLocation(shader, name), 1, &val[0]);
	}

	static void SetMat4(const Shader& shader, const char* name, const glm::mat4& val)
	{
		glUniformMatrix4fv(ShaderManager::GetUniformLocation(shader, name), 1, GL_FALSE, &val[0][0]);
	}

	static void SetMat4(const std::shared_ptr<Shader>& shader, const char* name, const glm::mat4& val)
	{
		glUniformMatrix4fv(ShaderManager::GetUniformLocation(shader, name), 1, GL_FALSE, &val[0][0]);
	}
	
	static void SetMat4(const std::shared_ptr<Shader>& shader, const char* name, const int count, const std::vector<glm::mat4>& val)
	{
		glUniformMatrix4fv(ShaderManager::GetUniformLocation(shader, name), count, GL_FALSE, &val[0][0][0]);
	}

private:
	ShaderManager()
	{
		//LOGGER(info, "shader manager init");
	}

	~ShaderManager()
	{
		//LOGGER(info, "shader manager release");
	}

	struct ShaderInfo
	{
		std::string name;
		std::string vsCode;
		std::string fsCode;
	};

	std::vector<ShaderInfo> _predefinedShaders;

};