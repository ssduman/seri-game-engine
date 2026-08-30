#include "Seripch.h"

#include "seri/util/Util.h"
#include "seri/camera/CameraBase.h"
#include "seri/graphic/Graphic.h"
#include "seri/graphic/Mesh.h"
#include "seri/graphic/Model.h"
#include "seri/graphic/Material.h"
#include "seri/rendering/render/RenderingManager.h"

namespace seri
{
	void Graphic::AddCamera(std::shared_ptr<CameraBase> camera)
	{
		GetInstance()._cameras.push_back(camera);

		if (camera->IsOrtho())
		{
			if (GetInstance()._cameraOrtho != nullptr)
			{
				LOGGER(warning, "there is already ortho camera");
				return;
			}

			GetInstance()._cameraOrtho = camera;
		}
		else
		{
			if (GetInstance()._cameraPerspective != nullptr)
			{
				LOGGER(warning, "there is already perspective camera");
				return;
			}

			GetInstance()._cameraPerspective = camera;
		}
	}

	std::shared_ptr<CameraBase> Graphic::GetCameraOrtho()
	{
		return GetInstance()._cameraOrtho;
	}

	std::shared_ptr<CameraBase> Graphic::GetCameraPerspective()
	{
		return GetInstance()._cameraPerspective;
	}

	void Graphic::DrawModel(const std::shared_ptr<Model>& model, const std::shared_ptr<Material>& material, const glm::mat4& trs)
	{
		for (const auto& mesh : model->meshes)
		{
			Graphic::Draw(mesh, material, trs);
		}
	}

	void Graphic::Draw(const std::shared_ptr<Mesh>& mesh, const std::shared_ptr<Material>& material, const glm::mat4& trs, PassType passType)
	{
		material->SetFloat4(literals::kUniformColor, glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });
		material->SetFloat3(literals::kUniformLightDir, glm::vec3{ 0.0f, 0.0f, -1.0f });
		material->SetFloat4(literals::kUniformLightColor, glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });

		if (mesh->bonesForVertices.size() > 0)
		{
			for (auto i = 0; i < SERI_MAX_BONES; i++)
			{
				std::string loc = fmt::format("{}[{}]", literals::kUniformBones, std::to_string(i));
				glm::mat4 transform = glm::mat4{ 1.0f };
				if (mesh->bones.find(i) != mesh->bones.end())
				{
					transform = mesh->bones[i].transform;
				}
				material->SetMat4(loc.c_str(), transform);
			}
		}

		RenderItem cmd{};
		cmd.type = passType;
		cmd.name = "draw";
		cmd.material = material;
		cmd.model = trs * mesh->transformation;
		cmd.vao = mesh->GetVao();
		RenderingManager::Submit(cmd);
	}

	void Graphic::DrawInstanced(const std::shared_ptr<Mesh>& mesh, const std::shared_ptr<Material>& material, const std::vector<glm::mat4>& trs)
	{
		material->SetFloat4(literals::kUniformColor, glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });
		material->SetFloat3(literals::kUniformLightDir, glm::vec3{ 0.0f, 0.0f, -1.0f });
		material->SetFloat4(literals::kUniformLightColor, glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });

		RenderItem cmd{};
		cmd.type = PassType::opaque;
		cmd.name = "draw_instanced";
		cmd.material = material;
		cmd.vao = mesh->GetVao();
		cmd.draw.mode = DrawMode::elements_instanced;
		cmd.draw.instanceCount = static_cast<uint32_t>(trs.size());
		RenderingManager::Submit(cmd);
	}

}
