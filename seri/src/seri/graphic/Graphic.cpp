#include "Seripch.h"

#include "seri/graphic/Graphic.h"
#include "seri/logging/Logger.h"
#include "seri/graphic/Mesh.h"
#include "seri/graphic/Model.h"
#include "seri/graphic/Material.h"
#include "seri/camera/ICamera.h"

namespace seri
{
	void Graphic::AddCamera(std::shared_ptr<ICamera> camera)
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

	std::shared_ptr<ICamera> Graphic::GetCameraOrtho()
	{
		return GetInstance()._cameraOrtho;
	}

	std::shared_ptr<ICamera> Graphic::GetCameraPerspective()
	{
		return GetInstance()._cameraPerspective;
	}

	void Graphic::Draw(const std::unique_ptr<Mesh>& mesh, const glm::mat4& trs, std::shared_ptr<Material>& material, std::shared_ptr<ICamera>& camera)
	{
		ShaderManager::Use(material->shader);
		if (material->texture != nullptr)
		{
			material->texture->Bind();
		}
		mesh->Bind();

		ShaderManager::SetUInt(material->shader, "u_texture", 0);
		ShaderManager::SetColor(material->shader, "u_color", glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });
		ShaderManager::SetVec3(material->shader, "u_view_pos", camera->GetCameraProperties().position);
		ShaderManager::SetVec3(material->shader, "u_light_dir", glm::vec3{ 0.0f, 0.0f, -1.0f });
		ShaderManager::SetColor(material->shader, "u_light_color", glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });
		ShaderManager::SetMat4(material->shader, "u_model", trs * mesh->transformation);
		ShaderManager::SetMat4(material->shader, "u_view", camera->GetView());
		ShaderManager::SetMat4(material->shader, "u_projection", camera->GetProjection());

		if (mesh->bonesForVertices.size() > 0)
		{
			//for (const auto& bone : mesh->bones)
			//{
			//	std::string loc = "u_bones[" + std::to_string(bone.second.index) + "]";
			//	ShaderManager::SetMat4(material->shader, loc.c_str(), bone.second.transform);
			//}
			for (auto i = 0; i < SERI_MAX_BONES; i++)
			{
				std::string loc = "u_bones[" + std::to_string(i) + "]";
				glm::mat4 transform = glm::mat4{ 1.0f };
				if (mesh->bones.find(i) != mesh->bones.end())
				{
					transform = mesh->bones[i].transform;
				}
				ShaderManager::SetMat4(material->shader, loc.c_str(), transform);
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
		ShaderManager::Disuse();
	}

	void Graphic::DrawModel(const std::unique_ptr<Model>& model, const glm::mat4& trs, std::shared_ptr<Material>& material, std::shared_ptr<ICamera>& camera)
	{
		for (const auto& mesh : model->meshes)
		{
			Graphic::Draw(mesh, trs, material, camera);
		}
	}

	void Graphic::DrawInstanced(const std::unique_ptr<Mesh>& mesh, const std::vector<glm::mat4>& trs, std::shared_ptr<Material>& material, std::shared_ptr<ICamera>& camera)
	{
		ShaderManager::Use(material->shader);
		if (material->texture != nullptr)
		{
			material->texture->Bind();
		}
		mesh->Bind();

		mesh->MakeInstanced(trs);

		ShaderManager::SetUInt(material->shader, "u_texture", 0);
		ShaderManager::SetColor(material->shader, "u_color", glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });
		ShaderManager::SetVec3(material->shader, "u_view_pos", camera->GetCameraProperties().position);
		ShaderManager::SetVec3(material->shader, "u_light_dir", glm::vec3{ 1.0f, 0.0f, 0.0f });
		ShaderManager::SetColor(material->shader, "u_light_color", glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });
		ShaderManager::SetMat4(material->shader, "u_view", camera->GetView());
		ShaderManager::SetMat4(material->shader, "u_projection", camera->GetProjection());

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
				ShaderManager::SetMat4(material->shader, loc.c_str(), transform);
			}
		}

		DrawElementsInstanced(mesh->count, static_cast<GLsizei>(trs.size()));

		mesh->Unbind();
		if (material->texture != nullptr)
		{
			material->texture->Unbind();
		}
		ShaderManager::Disuse();
	}

}
