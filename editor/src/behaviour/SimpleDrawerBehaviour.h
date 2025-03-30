#pragma once

#include <seri/logging/Logger.h>
#include <seri/behaviour/IBehaviour.h>

class SimpleDrawerBehaviour : public IBehaviour
{
public:
	void Init() override
	{
		LOGGER(info, "SimpleDrawerBehaviour Init");

		quad_2d = MeshG::quad_2d();
		quad_3d = MeshG::quad_3d();
		cube_3d = MeshG::cube_3d();
		line = MeshG::line();

		auto entityShader = ShaderManager::Find("entity");

		auto passageTexture = std::make_shared<Texture>();
		passageTexture->init("assets/textures/passage.png");

		material = std::make_shared<MaterialG>();
		material->shader = entityShader;
		material->texture = passageTexture;
	}

	void Update() override
	{
		//LOGGER(info, "SimpleDrawerBehaviour Update");

		glm::vec3 pos_2d{ 660.0f, 660.0f, 0.0f };
		glm::quat rot_2d = glm::quat(glm::vec3{ glm::radians(0.0f), glm::radians(0.0f), glm::radians(0.0f) });
		glm::vec3 scale_2d{ 100.0f, 100.0f, 100.0f };

		glm::vec3 pos_3d{ 1.0f, 0.0f, 1.0f };
		glm::quat rot_3d = glm::quat(glm::vec3{ glm::radians(0.0f), glm::radians(0.0f), glm::radians(0.0f) });
		glm::vec3 scale_3d{ 1.0f, 1.0f, 1.0f };

		glm::vec3 pos_line{ 0.0f, 0.0f, 0.0f };
		glm::quat rot_line = glm::quat(glm::vec3{ glm::radians(0.0f), glm::radians(0.0f), glm::radians(0.0f) });
		glm::vec3 scale_line{ 10.0f, 10.0f, 10.0f };

		//Graphic::Draw(line, Util::GetTRS(pos_line, rot_line, scale_line), material, Graphic::GetCameraOrtho());
		//Graphic::Draw(quad_2d, Util::GetTRS(pos_2d, rot_2d, scale_2d), material, Graphic::GetCameraOrtho());
		//Graphic::Draw(quad_3d, Util::GetTRS(pos_3d, rot_3d, scale_3d), material, Graphic::GetCameraPerspective());
		Graphic::Draw(cube_3d, Util::GetTRS(pos_3d, rot_3d, scale_3d), material, Graphic::GetCameraPerspective());
	}

	void Destroy() override
	{
		LOGGER(info, "SimpleDrawerBehaviour Destroy");
	}

private:
	std::shared_ptr<MeshG> quad_2d;
	std::shared_ptr<MeshG> quad_3d;
	std::shared_ptr<MeshG> cube_3d;
	std::shared_ptr<MeshG> line;

	std::shared_ptr<MaterialG> material;

};
