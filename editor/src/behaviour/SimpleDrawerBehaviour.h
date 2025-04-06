#pragma once

#include <seri/logging/Logger.h>
#include <seri/graphic/Mesh.h>
#include <seri/graphic/Material.h>
#include <seri/behaviour/BehaviourBase.h>
#include <seri/model/ModelImporter.h>

class SimpleDrawerBehaviour : public BehaviourBase
{
public:
	void Init() override
	{
		quad_2d = Mesh::quad_2d();
		quad_3d = Mesh::quad_3d();
		cube_3d = Mesh::cube_3d();
		line_2d = Mesh::line_2d(
			{ Graphic::GetCameraOrtho()->getCameraProperties().width / 2.0f, Graphic::GetCameraOrtho()->getCameraProperties().height / 2.0f },
			{ Graphic::GetCameraOrtho()->getCameraProperties().width, Graphic::GetCameraOrtho()->getCameraProperties().height }
		);

		//models_0 = ModelImporter{}.Load("assets/models/spider.obj");
		//models_0 = ModelImporter{}.Load("assets/models/tank.fbx");
		//models_0 = ModelImporter{}.Load("assets/models/blendshape.fbx");
		//models_0 = ModelImporter{}.Load("assets/models/prop_0.fbx");
		//models_0 = ModelImporter{}.Load("assets/models/prop_0_crk.fbx");
		//models_0 = ModelImporter{}.Load("assets/models/X Bot@Hip Hop Dancing.fbx");
		models_0 = ModelImporter{}.Load("assets/models/Hip Hop Dancing.dae");

		auto entityShader = ShaderManager::Find("entity");
		auto entitySkinnedShader = ShaderManager::Find("entity_skinned");
		auto lineShader = ShaderManager::Find("line");
		auto gridShader = ShaderManager::Find("grid");

		auto passageTexture = std::make_shared<Texture>();
		passageTexture->init("assets/textures/passage.png");

		auto tankTexture = std::make_shared<Texture>();
		tankTexture->init("assets/textures/tank/tank_diffuse.tga");

		material = std::make_shared<Material>();
		material->shader = entityShader;
		material->texture = passageTexture;

		materialLine = std::make_shared<Material>();
		materialLine->shader = lineShader;
		materialLine->texture = nullptr;

		materialGrid = std::make_shared<Material>();
		materialGrid->shader = gridShader;
		materialGrid->texture = nullptr;

		materialModel = std::make_shared<Material>();
		materialModel->shader = entityShader;
		materialModel->texture = tankTexture;

		materialSkinned = std::make_shared<Material>();
		materialSkinned->shader = entitySkinnedShader;
		materialSkinned->texture = tankTexture;
	}

	void Update() override
	{
		glm::vec3 pos_2d{ 660.0f, 660.0f, 0.0f };
		glm::quat rot_2d = glm::quat(glm::vec3{ glm::radians(0.0f), glm::radians(0.0f), glm::radians(0.0f) });
		glm::vec3 scale_2d{ 100.0f, 100.0f, 100.0f };

		glm::vec3 pos_3d{ 0.0f, 0.0f, 0.0f };
		glm::quat rot_3d = glm::quat(glm::vec3{ glm::radians(0.0f), glm::radians(0.0f), glm::radians(0.0f) });
		//glm::vec3 scale_3d{ 100.0f, 100.0f, 100.0f };
		glm::vec3 scale_3d{ 1.0f, 1.0f, 1.0f };
		//glm::vec3 scale_3d{ 0.01f, 0.01f, 0.01f };

		//Graphic::Draw(line_2d, Util::GetIdentityMatrix(), materialLine, Graphic::GetCameraOrtho());
		//Graphic::Draw(quad_3d, Util::GetTRS(pos_3d, rot_3d, scale_3d), material, Graphic::GetCameraPerspective());
		//Graphic::Draw(cube_3d, Util::GetTRS(pos_3d, rot_3d, scale_3d), material, Graphic::GetCameraPerspective());

		for (const auto& model : models_0)
		{
			model->UpdateAnimation();
			Graphic::Draw(model, Util::GetTRS(pos_3d, rot_3d, scale_3d), materialSkinned, Graphic::GetCameraPerspective());
		}

		Graphic::Draw(quad_2d, Util::GetIdentityMatrix(), materialGrid, Graphic::GetCameraPerspective());
	}

	void Destroy() override
	{}

private:
	std::unique_ptr<Mesh> quad_2d;
	std::unique_ptr<Mesh> quad_3d;
	std::unique_ptr<Mesh> cube_3d;
	std::unique_ptr<Mesh> line_2d;

	std::vector<std::unique_ptr<Mesh>> models_0;

	std::shared_ptr<Material> material;
	std::shared_ptr<Material> materialLine;
	std::shared_ptr<Material> materialGrid;
	std::shared_ptr<Material> materialModel;
	std::shared_ptr<Material> materialSkinned;

};
