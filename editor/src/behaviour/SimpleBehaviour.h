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

class SimpleBehaviour : public seri::BehaviourBase
{
public:
	void Init() override
	{
		quad_2d = seri::Mesh::quad_2d();
		quad_3d = seri::Mesh::quad_3d();
		cube_3d = seri::Mesh::cube_3d();
		font_mesh = std::make_unique<seri::Mesh>();

		model_0 = seri::ModelImporter{}.Load("assets/spiderman/source/spiderman.fbx");

		auto texture_0 = seri::TextureBase::Create();
		texture_0->Init(seri::TextureDesc{}, "assets/textures/passage.png");

		auto texture_1 = seri::TextureBase::Create();
		texture_1->Init(seri::TextureDesc{}, "assets/spiderman/textures/spiderman.png");

		const int fontIndex = 2;
		const auto& fontInfo = seri::font::FontManager::GetPredefinedFonts()[fontIndex]->fontInfo;
		const auto& fontTexture = seri::font::FontManager::GetPredefinedFonts()[fontIndex]->texture;

		materialFont = std::make_shared<seri::Material>();
		materialFont->shader = seri::ShaderLibrary::Find("typer");
		materialFont->SetTexture("u_texture", fontTexture);

		materialGrid = std::make_shared<seri::Material>();
		materialGrid->shader = seri::ShaderLibrary::Find("grid");

		materialInstanced = std::make_shared<seri::Material>();
		materialInstanced->shader = seri::ShaderLibrary::Find("entity_instanced");
		materialInstanced->SetTexture("u_texture", texture_0);

		for (size_t i = 0; model_0 && i < model_0->meshes.size(); i++)
		{
			auto& materialSkinned = std::make_shared<seri::Material>();
			materialSkinned->shader = seri::ShaderLibrary::Find("entity_skinned");
			materialSkinned->SetTexture("u_texture", texture_1);
			materialsSkinned.emplace_back(materialSkinned);
		}

		udpSocketServer = std::make_unique<seri::netcode::Socket>(seri::netcode::SocketType::udp);
		udpSocketServer->Bind({ "localhost", 5200 });

		udpSocketClient = std::make_unique<seri::netcode::Socket>(seri::netcode::SocketType::udp);
		udpSocketClient->Connect({ "127.0.0.1", 5200 });

		seri::font::FontManager::MakeText(font_mesh, fontInfo, "hello world", 200, 200);

		for (unsigned int i = 0; i < 100; i++)
		{
			instancedTRSs.push_back(seri::Util::GetTRS({ 4.0f + i % 20, i / 20, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }));
		}
		cube_3d->MakeInstanced(instancedTRSs);
	}

	void Update() override
	{
		glm::vec3 pos_3d{ 0.0f, 0.0f, 0.0f };
		glm::quat rot_3d = glm::quat(glm::vec3{ glm::radians(0.0f), glm::radians(0.0f), glm::radians(0.0f) });
		glm::vec3 scale_3d{ 1.0f, 1.0f, 1.0f };

		for (size_t i = 0; i < instancedTRSs.size(); i++)
		{
			float row = i % 20;
			float col = i / 20;
			instancedTRSs[i] *= glm::mat4_cast(glm::quat(glm::vec3{ glm::radians(seri::Random::Range(0.5f, 2.0f)), glm::radians(seri::Random::Range(0.5f, 2.0f)), glm::radians(0.0f) }));
		}
		cube_3d->UpdateInstanced(instancedTRSs);
		seri::Graphic::DrawInstanced(cube_3d, materialInstanced, instancedTRSs, seri::Graphic::GetCameraPerspective());

		if (model_0)
		{
			model_0->UpdateAnimations();

			for (size_t m = 0; m < model_0->meshes.size(); m++)
			{
				const auto& mesh = model_0->meshes[m];
				const auto& materialSkinned = materialsSkinned[m];
				seri::Graphic::Draw(mesh, materialSkinned, seri::Util::GetTRS(pos_3d, rot_3d, scale_3d), seri::Graphic::GetCameraPerspective());
			}
		}

		seri::Graphic::Draw(font_mesh, materialFont, seri::Util::GetIdentityMatrix(), seri::Graphic::GetCameraOrtho());

		//seri::Graphic::Draw(quad_2d, materialGrid, seri::Util::GetIdentityMatrix(), seri::Graphic::GetCameraPerspective());

		//udpSocketServer->Listen(1);
		//udpSocketClient->SendToServer();
	}

	void Destroy() override
	{
	}

private:
	std::unique_ptr<seri::Mesh> quad_2d;
	std::unique_ptr<seri::Mesh> quad_3d;
	std::unique_ptr<seri::Mesh> cube_3d;
	std::unique_ptr<seri::Mesh> font_mesh;

	std::unique_ptr<seri::Model> model_0;

	std::shared_ptr<seri::Material> materialFont;
	std::shared_ptr<seri::Material> materialGrid;
	std::shared_ptr<seri::Material> materialInstanced;
	std::vector<std::shared_ptr<seri::Material>> materialsSkinned;

	std::vector<glm::mat4> instancedTRSs;

	std::unique_ptr<seri::netcode::Socket> udpSocketServer;
	std::unique_ptr<seri::netcode::Socket> udpSocketClient;

};
