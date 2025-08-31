#pragma once

#include <seri/core/Seri.h>

#include <seri/light/Light.h>
#include <seri/graphic/Mesh.h>
#include <seri/graphic/Model.h>
#include <seri/graphic/Material.h>
#include <seri/behaviour/BehaviourBase.h>
#include <seri/model/ModelImporter.h>
#include <seri/netcode/Socket.h>
#include <seri/random/Random.h>

class SimpleDrawerBehaviour : public seri::BehaviourBase
{
public:
	void Init() override
	{
		quad_2d = seri::Mesh::quad_2d();
		quad_3d = seri::Mesh::quad_3d();
		cube_3d = seri::Mesh::cube_3d();
		line_2d = seri::Mesh::line_2d(
			{ seri::Graphic::GetCameraOrtho()->GetCameraProperties().width / 2.0f, seri::Graphic::GetCameraOrtho()->GetCameraProperties().height / 2.0f },
			{ seri::Graphic::GetCameraOrtho()->GetCameraProperties().width, seri::Graphic::GetCameraOrtho()->GetCameraProperties().height }
		);
		font_mesh = std::make_unique<seri::Mesh>();

		model_0 = seri::ModelImporter{}.Load("assets/spiderman/source/Thriller Part 4.fbx");

		auto entityShader = seri::ShaderLibrary::Find("entity");
		auto entityInstancedShader = seri::ShaderLibrary::Find("entity_instanced");
		auto entitySkinnedShader = seri::ShaderLibrary::Find("entity_skinned");
		auto lineShader = seri::ShaderLibrary::Find("line");
		auto gridShader = seri::ShaderLibrary::Find("grid");
		auto typerShader = seri::ShaderLibrary::Find("typer");

		auto texture_0 = std::make_shared<seri::Texture>();
		texture_0->Init("assets/textures/passage.png");

		auto texture_1 = std::make_shared<seri::Texture>();
		texture_1->Init("assets/spiderman/textures/spiderman.png");

		const int fontIndex = 2;
		const auto& fontInfo = seri::font::FontManager::GetPredefinedFonts()[fontIndex]->fontInfo;
		const auto& fontTexture = seri::font::FontManager::GetPredefinedFonts()[fontIndex]->texture;

		material = std::make_shared<seri::Material>();
		material->shader = entityShader;
		material->texture = texture_0;

		materialInstanced = std::make_shared<seri::Material>();
		materialInstanced->shader = entityInstancedShader;
		materialInstanced->texture = texture_0;

		materialLine = std::make_shared<seri::Material>();
		materialLine->shader = lineShader;
		materialLine->texture = nullptr;

		materialGrid = std::make_shared<seri::Material>();
		materialGrid->shader = gridShader;
		materialGrid->texture = nullptr;

		materialFont = std::make_shared<seri::Material>();
		materialFont->shader = typerShader;
		materialFont->texture = fontTexture;

		materialModel = std::make_shared<seri::Material>();
		materialModel->shader = entityShader;
		materialModel->texture = texture_1;

		materialSkinned = std::make_shared<seri::Material>();
		materialSkinned->shader = entitySkinnedShader;
		materialSkinned->texture = texture_1;

		udpSocketServer = std::make_unique<seri::netcode::Socket>(seri::netcode::SocketType::udp);
		udpSocketServer->Bind({ "localhost", 5200 });

		udpSocketClient = std::make_unique<seri::netcode::Socket>(seri::netcode::SocketType::udp);
		udpSocketClient->Connect({ "127.0.0.1", 5200 });

		seri::font::FontManager::MakeText(font_mesh, fontInfo, "hello world", 200, 200);

		for (unsigned int i = 0; i < 100; i++)
		{
			float row = i % 20;
			float col = i / 20;
			instancedTRSs.push_back(seri::Util::GetTRS(
				glm::vec3{ 4.0f + row, col, 0.0f },
				glm::quat(glm::vec3{ glm::radians(i * 6.0f), glm::radians(0.0f), glm::radians(0.0f) }),
				glm::vec3{ 0.1f, 0.1f, 0.1f }
			));
		}
	}

	void Update() override
	{
		glm::vec3 pos_2d{ 660.0f, 660.0f, 0.0f };
		glm::quat rot_2d = glm::quat(glm::vec3{ glm::radians(0.0f), glm::radians(0.0f), glm::radians(0.0f) });
		glm::vec3 scale_2d{ 100.0f, 100.0f, 100.0f };

		glm::vec3 pos_3d{ 0.0f, 0.0f, 0.0f };
		glm::quat rot_3d = glm::quat(glm::vec3{ glm::radians(0.0f), glm::radians(0.0f), glm::radians(0.0f) });
		glm::vec3 scale_3d{ 1.0f, 1.0f, 1.0f };
		glm::vec3 scale_model{ 1.01f, 1.01f, 1.01f };

		//seri::Graphic::Draw(line_2d, seri::Util::GetIdentityMatrix(), materialLine, seri::Graphic::GetCameraOrtho());
		//seri::Graphic::Draw(quad_3d, seri::Util::GetTRS(pos_3d, rot_3d, scale_3d), material, seri::Graphic::GetCameraPerspective());
		//seri::Graphic::Draw(cube_3d, seri::Util::GetTRS(pos_3d, rot_3d, scale_3d), material, seri::Graphic::GetCameraPerspective());

		for (unsigned int i = 0; i < 100; i++)
		{
			float row = i % 20;
			float col = i / 20;
			instancedTRSs[i] *= glm::mat4_cast(glm::quat(glm::vec3{ glm::radians(seri::Random::Range(0.5f, 2.0f)), glm::radians(seri::Random::Range(0.5f, 2.0f)), glm::radians(0.0f) }));
		}
		seri::Graphic::DrawInstanced(cube_3d, instancedTRSs, materialInstanced, seri::Graphic::GetCameraPerspective());

		if (model_0)
		{
			model_0->UpdateAnimations();
			seri::Graphic::DrawModel(model_0, seri::Util::GetTRS(pos_3d, rot_3d, scale_model), materialSkinned, seri::Graphic::GetCameraPerspective());
		}

		seri::Graphic::Draw(font_mesh, seri::Util::GetIdentityMatrix(), materialFont, seri::Graphic::GetCameraOrtho());

		//seri::Graphic::Draw(quad_2d, seri::Util::GetIdentityMatrix(), materialGrid, seri::Graphic::GetCameraPerspective());

		//udpSocketServer->Listen(1);
		//udpSocketClient->SendToServer();
	}

	void Destroy() override
	{
	}

private:
	seri::Light light;

	std::unique_ptr<seri::Mesh> quad_2d;
	std::unique_ptr<seri::Mesh> quad_3d;
	std::unique_ptr<seri::Mesh> cube_3d;
	std::unique_ptr<seri::Mesh> line_2d;
	std::unique_ptr<seri::Mesh> font_mesh;

	std::unique_ptr<seri::Model> model_0;

	std::shared_ptr<seri::Material> material;
	std::shared_ptr<seri::Material> materialInstanced;
	std::shared_ptr<seri::Material> materialLine;
	std::shared_ptr<seri::Material> materialGrid;
	std::shared_ptr<seri::Material> materialFont;
	std::shared_ptr<seri::Material> materialModel;
	std::shared_ptr<seri::Material> materialSkinned;

	std::unique_ptr<seri::netcode::Socket> udpSocketServer;
	std::unique_ptr<seri::netcode::Socket> udpSocketClient;

	std::vector<glm::mat4> instancedTRSs;

};
