#pragma once

#include "seri/camera/CameraBase.h"
#include "seri/shader/ShaderBase.h"

#include <memory>
#include <vector>
#include <filesystem>

namespace seri
{
	class ShaderLibrary
	{
	public:

		struct ShaderInfo
		{
			bool valid;
			std::string name;
			std::string vsCode;
			std::string fsCode;
		};

		ShaderLibrary(ShaderLibrary const&) = delete;

		void operator=(ShaderLibrary const&) = delete;

		static ShaderLibrary& GetInstance()
		{
			static ShaderLibrary instance;
			return instance;
		}

		static void Init(const char* shaderFolderPath)
		{
			GetInstance()._predefinedShaders = {};

			//for (const auto& entry : std::filesystem::directory_iterator(shaderFolderPath))
			//{
			//	std::string name = entry.path().stem().string();
			//	std::string text = Util::ReadFileAtPath(entry.path().string().c_str());

			//	ShaderInfo info;
			//	info.valid = true;
			//	info.name = name;
			//	info.vsCode = Util::GetContentOfToken(text, "#beg_vs", "#end_vs");
			//	info.fsCode = Util::GetContentOfToken(text, "#beg_fs", "#end_fs");
			//	GetInstance()._predefinedShaders[name] = info;
			//}

			LOGGER(info, "[shader library] init done");
		}

		static ShaderInfo& Get(const std::string& name)
		{
			if (GetInstance()._predefinedShaders.find(name) != GetInstance()._predefinedShaders.end())
			{
				return GetInstance()._predefinedShaders[name];
			}

			throw std::runtime_error("[shader library] not found: " + name);
		}

		static std::shared_ptr<ShaderBase> Find(const std::string& name)
		{
			auto& predef = GetInstance().Get(name);
			auto shader = ShaderBase::Create();
			shader->Init(predef.vsCode.c_str(), predef.fsCode.c_str(), /*readFromFile*/ false);
			return shader;
		}

		static void SetView(std::shared_ptr<ShaderBase>& shader, const glm::mat4& view)
		{
			shader->Bind();
			shader->SetMat4("u_view", view);
			shader->Unbind();
		}

		static void SetModel(std::shared_ptr<ShaderBase>& shader, const glm::mat4& model)
		{
			shader->Bind();
			shader->SetMat4("u_model", model);
			shader->Unbind();
		}

		static void SetProjection(std::shared_ptr<ShaderBase>& shader, const glm::mat4& projection)
		{
			shader->Bind();
			shader->SetMat4("u_projection", projection);
			shader->Unbind();
		}

		static void SetVP(std::shared_ptr<ShaderBase>& shader, const std::shared_ptr<CameraBase>& camera)
		{
			shader->Bind();
			shader->SetMat4("u_view", camera->GetView());
			shader->SetMat4("u_projection", camera->GetProjection());
			shader->Unbind();
		}

		static void SetMVP(std::shared_ptr<ShaderBase>& shader, const std::shared_ptr<CameraBase>& camera)
		{
			shader->Bind();
			shader->SetMat4("u_model", glm::mat4{ 1.0f });
			shader->SetMat4("u_view", camera->GetView());
			shader->SetMat4("u_projection", camera->GetProjection());
			shader->Unbind();
		}

		static void SetTRS(std::shared_ptr<ShaderBase>& shader, const std::shared_ptr<CameraBase>& camera)
		{
			shader->Bind();
			shader->SetMat4("u_model", glm::mat4{ 1.0f });
			shader->SetMat4("u_view", camera->GetView());
			shader->SetMat4("u_projection", camera->GetProjection());
			shader->Unbind();
		}

		static void SetPosition(std::shared_ptr<ShaderBase>& shader, const glm::vec2& position)
		{
			shader->Bind();
			shader->SetFloat2("u_position", position);
			shader->Unbind();
		}

		static void SetPosition(std::shared_ptr<ShaderBase>& shader, const glm::vec3& position)
		{
			shader->Bind();
			shader->SetFloat3("u_position", position);
			shader->Unbind();
		}

		static void SetColor(std::shared_ptr<ShaderBase>& shader, const glm::vec4& color)
		{
			shader->Bind();
			shader->SetFloat4("u_color", color);
			shader->Unbind();
		}

	private:
		ShaderLibrary()
		{
			//LOGGER(info, "[shader] manager init");
		}

		~ShaderLibrary()
		{
			//LOGGER(info, "[shader] manager release");
		}

		std::unordered_map<std::string, ShaderInfo> _predefinedShaders;

	};
}
