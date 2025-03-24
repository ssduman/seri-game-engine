#pragma once

#include "seri/graphic/MeshG.h"
#include "seri/graphic/MaterialG.h"
#include "seri/camera/ICamera.h"

#include <vector>
#include <memory>

class Graphic
{
public:
	Graphic() = default;

	~Graphic() = default;

	void draw(std::shared_ptr<MeshG> mesh, const glm::mat4& trs, std::shared_ptr<MaterialG> material, std::shared_ptr<ICamera> camera)
	{
		ShaderManager::Use(material->shader);
		material->texture->bind();
		mesh->bind();

		ShaderManager::SetMat4(material->shader, "u_model", trs);
		ShaderManager::SetMat4(material->shader, "u_view", camera->getView());
		ShaderManager::SetMat4(material->shader, "u_projection", camera->getProjection());

		if (mesh->hasIndex())
		{
			drawElements(mesh->count);
		}
		else
		{
			drawArrays(mesh->count);
		}

		mesh->unbind();
		material->texture->unbind();
		ShaderManager::Disuse();
	}

	void DrawMesh(std::shared_ptr<MeshG> mesh, std::shared_ptr<MaterialG> material, const glm::mat4& trs)
	{}

	void DrawMeshInstanced(std::shared_ptr<MeshG> mesh, std::shared_ptr<MaterialG> material, glm::mat4 trs[])
	{}

private:

	void drawElements(GLsizei count)
	{
		aux::DrawElement draw;
		draw.mode = aux::toGLenum(aux::DrawMode::triangles);
		draw.count = count;
		draw.type = aux::toGLenum(aux::Type::uint_type);
		draw.indices = nullptr;

		glDrawElements(draw.mode, draw.count, draw.type, draw.indices);
	}

	void drawArrays(GLsizei count)
	{
		aux::DrawArray draw;
		draw.mode = aux::toGLenum(aux::DrawMode::triangles);
		draw.first = 0;
		draw.count = count;

		glDrawArrays(draw.mode, draw.first, draw.count);
	}

};
