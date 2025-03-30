#pragma once

#include "seri/graphic/MeshG.h"
#include "seri/graphic/MaterialG.h"
#include "seri/camera/ICamera.h"

#include <vector>
#include <memory>

class Graphic
{
public:
	Graphic(Graphic const&) = delete;

	void operator=(Graphic const&) = delete;

	static void Init()
	{
		GetInstance();
		LOGGER(info, "graphic manager init done");
	}

	static Graphic& GetInstance()
	{
		static Graphic instance;
		return instance;
	}

	static void AddCamera(std::shared_ptr<ICamera> camera)
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

	static std::shared_ptr<ICamera> GetCameraOrtho()
	{
		return GetInstance()._cameraOrtho;
	}

	static std::shared_ptr<ICamera> GetCameraPerspective()
	{
		return GetInstance()._cameraPerspective;
	}

	static void Draw(std::shared_ptr<MeshG> mesh, const glm::mat4& trs, std::shared_ptr<MaterialG> material, std::shared_ptr<ICamera> camera)
	{
		ShaderManager::Use(material->shader);
		material->texture->bind();
		mesh->bind();

		ShaderManager::SetUInt(material->shader, "u_texture", 0);
		ShaderManager::SetColor(material->shader, "u_color", glm::vec4{ 1.0f, 0.0f, 0.0f, 1.0f });
		ShaderManager::SetMat4(material->shader, "u_model", trs);
		ShaderManager::SetMat4(material->shader, "u_view", camera->getView());
		ShaderManager::SetMat4(material->shader, "u_projection", camera->getProjection());

		if (mesh->hasIndex())
		{
			drawElements(mesh->count, mesh->drawMode);
		}
		else
		{
			drawArrays(mesh->count, mesh->drawMode);
		}

		mesh->unbind();
		material->texture->unbind();
		ShaderManager::Disuse();
	}

	static void DrawMesh(std::shared_ptr<MeshG> mesh, std::shared_ptr<MaterialG> material, const glm::mat4& trs)
	{}

	static void DrawMeshInstanced(std::shared_ptr<MeshG> mesh, std::shared_ptr<MaterialG> material, glm::mat4 trs[])
	{}

private:
	Graphic()
	{
		LOGGER(info, "graphic manager init");
	}

	~Graphic()
	{
		LOGGER(info, "graphic manager release");
	}

	static void drawElements(GLsizei count, aux::DrawMode drawMode = aux::DrawMode::triangles)
	{
		aux::DrawElement draw;
		draw.mode = aux::toGLenum(drawMode);
		draw.count = count;
		draw.type = aux::toGLenum(aux::Type::uint_type);
		draw.indices = nullptr;

		glDrawElements(draw.mode, draw.count, draw.type, draw.indices);
	}

	static void drawArrays(GLsizei count, aux::DrawMode drawMode = aux::DrawMode::triangles)
	{
		aux::DrawArray draw;
		draw.mode = aux::toGLenum(drawMode);
		draw.first = 0;
		draw.count = count;

		glDrawArrays(draw.mode, draw.first, draw.count);
	}

	std::shared_ptr<ICamera> _cameraOrtho;
	std::shared_ptr<ICamera> _cameraPerspective;
	std::vector<std::shared_ptr<ICamera>> _cameras;

};
