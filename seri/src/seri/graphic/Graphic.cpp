#include "Seripch.h"

#include "seri/graphic/Graphic.h"
#include "seri/graphic/Mesh.h"
#include "seri/graphic/Material.h"
#include "seri/camera/ICamera.h"

void Graphic::AddCamera(std::shared_ptr<ICamera> camera)
{
	GetInstance()._cameras.push_back(camera);

	if (camera->IsOrtho())
	{
		GetInstance()._cameraOrtho = camera;
	}
	else
	{
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

void Graphic::Draw(std::shared_ptr<Mesh> mesh, const glm::mat4& trs, std::shared_ptr<Material> material, std::shared_ptr<ICamera> camera)
{
	ShaderManager::Use(material->shader);
	if (material->texture != nullptr)
	{
		material->texture->bind();
	}
	mesh->Bind();

	ShaderManager::SetUInt(material->shader, "u_texture", 0);
	ShaderManager::SetColor(material->shader, "u_color", glm::vec4{ 1.0f, 0.0f, 0.0f, 1.0f });
	ShaderManager::SetMat4(material->shader, "u_model", trs * mesh->transformation);
	ShaderManager::SetMat4(material->shader, "u_view", camera->getView());
	ShaderManager::SetMat4(material->shader, "u_projection", camera->getProjection());

	if (mesh->HasIndex())
	{
		drawElements(mesh->count, mesh->drawMode);
	}
	else
	{
		drawArrays(mesh->count, mesh->drawMode);
	}

	mesh->Unbind();
	if (material->texture != nullptr)
	{
		material->texture->unbind();
	}
	ShaderManager::Disuse();
}
