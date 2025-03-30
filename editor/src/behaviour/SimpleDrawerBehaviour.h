#pragma once

#include <seri/logging/Logger.h>
#include <seri/graphic/MeshG.h>
#include <seri/graphic/MaterialG.h>
#include <seri/behaviour/IBehaviour.h>
#include <seri/model/ModelImporter.h>

class SimpleDrawerBehaviour : public IBehaviour
{
public:
	void Init() override
	{
		LOGGER(info, "SimpleDrawerBehaviour Init");

		quad_2d = MeshG::quad_2d();
		quad_3d = MeshG::quad_3d();
		cube_3d = MeshG::cube_3d();
		line_2d = MeshG::line_2d(
			{ Graphic::GetCameraOrtho()->getCameraProperties().width / 2.0f, Graphic::GetCameraOrtho()->getCameraProperties().height / 2.0f },
			{ Graphic::GetCameraOrtho()->getCameraProperties().width, Graphic::GetCameraOrtho()->getCameraProperties().height }
		);

		model_0 = ModelImporter{}.load("assets/models/spider.obj");

		auto entityShader = ShaderManager::Find("entity");
		auto lineShader = ShaderManager::Find("line");
		auto gridShader = ShaderManager::Find("grid");

		auto passageTexture = std::make_shared<Texture>();
		passageTexture->init("assets/textures/passage.png");

		material = std::make_shared<MaterialG>();
		material->shader = entityShader;
		material->texture = passageTexture;

		materialLine = std::make_shared<MaterialG>();
		materialLine->shader = lineShader;
		materialLine->texture = nullptr;

		materialGrid = std::make_shared<MaterialG>();
		materialGrid->shader = gridShader;
		materialGrid->texture = nullptr;
	}

	void Update() override
	{
		glm::vec3 pos_2d{ 660.0f, 660.0f, 0.0f };
		glm::quat rot_2d = glm::quat(glm::vec3{ glm::radians(0.0f), glm::radians(0.0f), glm::radians(0.0f) });
		glm::vec3 scale_2d{ 100.0f, 100.0f, 100.0f };

		glm::vec3 pos_3d{ 0.0f, 0.0f, 0.0f };
		glm::quat rot_3d = glm::quat(glm::vec3{ glm::radians(0.0f), glm::radians(0.0f), glm::radians(0.0f) });
		glm::vec3 scale_3d{ 0.1f, 0.1f, 0.1f };

		//Graphic::Draw(line_2d, Util::GetIdentityMatrix(), materialLine, Graphic::GetCameraOrtho());
		//Graphic::Draw(quad_3d, Util::GetTRS(pos_3d, rot_3d, scale_3d), material, Graphic::GetCameraPerspective());
		//Graphic::Draw(cube_3d, Util::GetTRS(pos_3d, rot_3d, scale_3d), material, Graphic::GetCameraPerspective());

		Graphic::Draw(model_0, Util::GetTRS(pos_3d, rot_3d, scale_3d), material, Graphic::GetCameraPerspective());

		Graphic::Draw(quad_2d, Util::GetIdentityMatrix(), materialGrid, Graphic::GetCameraPerspective());
	}

	void Destroy() override
	{
		LOGGER(info, "SimpleDrawerBehaviour Destroy");
	}

private:
	std::shared_ptr<MeshG> quad_2d;
	std::shared_ptr<MeshG> quad_3d;
	std::shared_ptr<MeshG> cube_3d;
	std::shared_ptr<MeshG> line_2d;

	std::shared_ptr<MeshG> model_0;

	std::shared_ptr<MaterialG> material;
	std::shared_ptr<MaterialG> materialLine;
	std::shared_ptr<MaterialG> materialGrid;

};
