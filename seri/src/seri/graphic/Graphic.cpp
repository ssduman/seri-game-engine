#include "Seripch.h"

#include "seri/util/Util.h"
#include "seri/camera/CameraBase.h"
#include "seri/graphic/Graphic.h"
#include "seri/graphic/Mesh.h"
#include "seri/graphic/Model.h"
#include "seri/graphic/Material.h"
#include "seri/rendering/RenderingManager.h"

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

	void Graphic::DrawModel(const std::unique_ptr<Model>& model, const std::shared_ptr<Material>& material, const glm::mat4& trs, const std::shared_ptr<CameraBase>& camera, const std::shared_ptr<FramebufferBase>& rt)
	{
		for (const auto& mesh : model->meshes)
		{
			Graphic::Draw(mesh, material, trs, camera);
		}
	}

	void Graphic::Draw(const std::unique_ptr<Mesh>& mesh, const std::shared_ptr<Material>& material, const glm::mat4& trs, const std::shared_ptr<CameraBase>& camera, const std::shared_ptr<FramebufferBase>& rt)
	{
		RenderingManager::Begin(camera);

		material->SetFloat4("u_color", glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });
		material->SetFloat3("u_view_pos", camera->GetCameraProperties().position);
		material->SetFloat3("u_light_dir", glm::vec3{ 0.0f, 0.0f, -1.0f });
		material->SetFloat4("u_light_color", glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });

		if (mesh->bonesForVertices.size() > 0)
		{
			for (auto i = 0; i < SERI_MAX_BONES; i++)
			{
				std::string loc = "u_bones[" + std::to_string(i) + "]";
				glm::mat4 transform = glm::mat4{ 1.0f };
				if (mesh->bones.find(i) != mesh->bones.end())
				{
					transform = mesh->bones[i].transform;
				}
				material->SetMat4(loc.c_str(), transform);
			}
		}

		seri::RenderCommand renderCommand{};
		renderCommand.name = "draw";
		renderCommand.camera = camera;
		renderCommand.material = material;
		renderCommand.vao = mesh->GetVao();
		renderCommand.rt = rt;
		renderCommand.trs = trs * mesh->transformation;
		seri::RenderingManager::Submit(renderCommand);

		RenderingManager::End();
	}

	void Graphic::DrawInstanced(const std::unique_ptr<Mesh>& mesh, const std::shared_ptr<Material>& material, const std::vector<glm::mat4>& trs, const std::shared_ptr<CameraBase>& camera)
	{
		seri::RenderingManager::Begin(camera);

		material->SetFloat4("u_color", glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });
		material->SetFloat3("u_view_pos", camera->GetCameraProperties().position);
		material->SetFloat3("u_light_dir", glm::vec3{ 0.0f, 0.0f, -1.0f });
		material->SetFloat4("u_light_color", glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });

		seri::RenderCommand renderCommand{};
		renderCommand.name = "draw_instanced";
		renderCommand.camera = camera;
		renderCommand.material = material;
		renderCommand.vao = mesh->GetVao();
		renderCommand.drawMode = seri::DrawMode::elements_instanced;
		renderCommand.instanceCount = static_cast<uint32_t>(trs.size());
		seri::RenderingManager::Submit(renderCommand);

		seri::RenderingManager::End();
	}

}
