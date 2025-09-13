#include "Seripch.h"

#include "seri/util/Util.h"
#include "seri/graphic/Graphic.h"
#include "seri/graphic/Mesh.h"
#include "seri/graphic/Model.h"
#include "seri/graphic/Material.h"

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

#if false
	void Graphic::Draw(const std::unique_ptr<Mesh>& mesh, const glm::mat4& trs, std::shared_ptr<Material>& material, std::shared_ptr<CameraBase>& camera)
	{
		material->shader->Bind();
		if (material->texture != nullptr)
		{
			material->texture->Bind();
		}
		mesh->Bind();

		material->shader->SetInt("u_texture", 0);
		material->shader->SetFloat3("u_view_pos", camera->GetCameraProperties().position);
		material->shader->SetFloat3("u_light_dir", glm::vec3{ 0.0f, 0.0f, -1.0f });
		material->shader->SetFloat4("u_color", glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });
		material->shader->SetFloat4("u_light_color", glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });
		material->shader->SetMat4("u_model", trs * mesh->transformation);
		material->shader->SetMat4("u_view", camera->GetView());
		material->shader->SetMat4("u_projection", camera->GetProjection());

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
				material->shader->SetMat4(loc.c_str(), transform);
			}
		}

		if (mesh->HasIndex())
		{
			DrawElements(mesh->count, mesh->drawMode);
		}
		else
		{
			DrawArrays(mesh->count, mesh->drawMode);
		}

		mesh->Unbind();
		if (material->texture != nullptr)
		{
			material->texture->Unbind();
		}
		material->shader->Unbind();
	}

	void Graphic::DrawModel(const std::unique_ptr<Model>& model, const glm::mat4& trs, std::shared_ptr<Material>& material, std::shared_ptr<CameraBase>& camera)
	{
		for (const auto& mesh : model->meshes)
		{
			Graphic::Draw(mesh, trs, material, camera);
		}
	}

	void Graphic::DrawInstanced(const std::unique_ptr<Mesh>& mesh, const std::vector<glm::mat4>& trs, std::shared_ptr<Material>& material, std::shared_ptr<CameraBase>& camera)
	{
		material->shader->Bind();
		if (material->texture != nullptr)
		{
			material->texture->Bind();
		}
		mesh->Bind();

		mesh->MakeInstanced(trs);

		material->shader->SetInt("u_texture", 0);
		material->shader->SetFloat3("u_view_pos", camera->GetCameraProperties().position);
		material->shader->SetFloat3("u_light_dir", glm::vec3{ 1.0f, 0.0f, 0.0f });
		material->shader->SetFloat4("u_color", glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });
		material->shader->SetFloat4("u_light_color", glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });
		material->shader->SetMat4("u_view", camera->GetView());
		material->shader->SetMat4("u_projection", camera->GetProjection());

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
				material->shader->SetMat4(loc.c_str(), transform);
			}
		}

		DrawElementsInstanced(mesh->count, static_cast<GLsizei>(trs.size()));

		mesh->Unbind();
		if (material->texture != nullptr)
		{
			material->texture->Unbind();
		}
		material->shader->Unbind();
	}
#endif

}
