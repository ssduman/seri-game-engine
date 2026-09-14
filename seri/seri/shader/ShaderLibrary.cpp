#include "Seripch.h"

#include "seri/shader/ShaderLibrary.h"

namespace seri
{
	void ShaderLibrary::Clear()
	{
		GetInstance()._predefinedGLSLs = {};
		GetInstance()._predefinedShaders = {};
	}

	void ShaderLibrary::AddShader(const std::filesystem::path& path)
	{
		std::string name = path.stem().string();
		if (GetInstance()._predefinedShaders.contains(name))
		{
			LIB_LOGGER(warning, shader_library) << "duplicate shader name '" << name << "', skipping " << path.string();
			return;
		}

		std::string text = Util::ReadFileAtPath(path.string().c_str());

		ShaderInfo info;
		info.valid = true;
		info.name = name;
		info.vsCode = Util::GetContentOfToken(text, "#beg_vs", "#end_vs");
		info.fsCode = Util::GetContentOfToken(text, "#beg_fs", "#end_fs");
		GetInstance()._predefinedShaders[name] = info;
	}

	void ShaderLibrary::AddGLSL(const std::filesystem::path& path)
	{
		std::string name = path.filename().string();
		if (GetInstance()._predefinedGLSLs.contains(name))
		{
			LIB_LOGGER(warning, shader_library) << "duplicate glsl name '" << name << "', skipping " << path.string();
			return;
		}

		std::string text = Util::ReadFileAtPath(path.string().c_str());

		if (Util::Contains(text, "#include"))
		{
			LIB_LOGGER(error, shader_library) << "glsl include not supported in glsl: " << name;
			return;
		}

		GLSLInfo info;
		info.valid = true;
		info.name = path.stem().string();
		info.code = text;
		GetInstance()._predefinedGLSLs[name] = info;
	}

	ShaderLibrary::GLSLInfo& ShaderLibrary::GetGLSL(const std::string& name)
	{
		if (GetInstance()._predefinedGLSLs.find(name) != GetInstance()._predefinedGLSLs.end())
		{
			return GetInstance()._predefinedGLSLs[name];
		}

		throw std::runtime_error("[shader library] not found: " + name);
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
