#include "Seripch.h"

#include "seri/shader/ShaderLibrary.h"

namespace seri
{
	ShaderLibrary& ShaderLibrary::GetInstance()
	{
		static ShaderLibrary instance;
		return instance;
	}

	void ShaderLibrary::Init(const char* shaderFolderPath)
	{
		GetInstance()._predefinedShaders = {};

		for (const auto& entry : std::filesystem::directory_iterator(shaderFolderPath))
		{
			if (Util::Contains(entry.path().string(), ".smeta"))
			{
				continue;
			}

			std::string name = entry.path().stem().string();
			std::string text = Util::ReadFileAtPath(entry.path().string().c_str());

			ShaderInfo info;
			info.valid = true;
			info.name = name;
			info.vsCode = Util::GetContentOfToken(text, "#beg_vs", "#end_vs");
			info.fsCode = Util::GetContentOfToken(text, "#beg_fs", "#end_fs");
			GetInstance()._predefinedShaders[name] = info;
		}

		LOGGER(info, fmt::format("[shader library] init done, count: {}", GetInstance()._predefinedShaders.size()));
	}

	ShaderLibrary::ShaderInfo& ShaderLibrary::Get(const std::string& name)
	{
		if (GetInstance()._predefinedShaders.find(name) != GetInstance()._predefinedShaders.end())
		{
			return GetInstance()._predefinedShaders[name];
		}

		throw std::runtime_error("[shader library] not found: " + name);
	}

	std::shared_ptr<ShaderBase> ShaderLibrary::Find(const std::string& name)
	{
		auto& predef = GetInstance().Get(name);
		auto shader = ShaderBase::Create();
		shader->Init(predef.vsCode.c_str(), predef.fsCode.c_str());
		return shader;
	}

	void ShaderLibrary::SetView(std::shared_ptr<ShaderBase>& shader, const glm::mat4& view)
	{
		shader->Bind();
		shader->SetMat4(literals::kUniformView, view);
		shader->Unbind();
	}

	void ShaderLibrary::SetProjection(std::shared_ptr<ShaderBase>& shader, const glm::mat4& projection)
	{
		shader->Bind();
		shader->SetMat4(literals::kUniformProjection, projection);
		shader->Unbind();
	}

	void ShaderLibrary::SetMVP(std::shared_ptr<ShaderBase>& shader, const std::shared_ptr<CameraBase>& camera)
	{
		shader->Bind();
		shader->SetMat4(literals::kUniformModel, glm::mat4{ 1.0f });
		shader->SetMat4(literals::kUniformView, camera->GetView());
		shader->SetMat4(literals::kUniformProjection, camera->GetProjection());
		shader->Unbind();
	}

}
