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

		model_0 = seri::ModelImporter{}.Load("assets/spiderman/source/Thriller Part 4.fbx");

		auto texture_0 = seri::TextureBase::Create();
		texture_0->Init(seri::TextureDesc{}, "assets/textures/passage.png");

		auto texture_1 = seri::TextureBase::Create();
		texture_1->Init(seri::TextureDesc{}, "assets/spiderman/textures/spiderman.png");

		const int fontIndex = 2;
		const auto& fontInfo = seri::font::FontManager::GetPredefinedFonts()[fontIndex]->fontInfo;
		const auto& fontTexture = seri::font::FontManager::GetPredefinedFonts()[fontIndex]->texture;

		material = std::make_shared<seri::Material>();
		material->shader = seri::ShaderLibrary::Find("entity");
		material->SetTexture("u_texture", texture_0);

		materialInstanced = std::make_shared<seri::Material>();
		materialInstanced->shader = seri::ShaderLibrary::Find("entity_instanced");
		materialInstanced->SetTexture("u_texture", texture_0);

		materialGrid = std::make_shared<seri::Material>();
		materialGrid->shader = seri::ShaderLibrary::Find("grid");
		materialGrid->texture = nullptr;

		materialFont = std::make_shared<seri::Material>();
		materialFont->shader = seri::ShaderLibrary::Find("typer");
		materialFont->texture = fontTexture;

		materialModel = std::make_shared<seri::Material>();
		materialModel->shader = seri::ShaderLibrary::Find("entity");
		materialModel->texture = texture_1;

		for (size_t i = 0; i < model_0->meshes.size(); i++)
		{
			auto& materialSkinned = std::make_shared<seri::Material>();
			materialSkinned->shader = seri::ShaderLibrary::Find("entity_skinned");
			materialSkinned->texture = texture_1;
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
			float row = i % 20;
			float col = i / 20;
			instancedTRSs.push_back(
				seri::Util::GetTRS(
					glm::vec3{ 4.0f + row, col, 0.0f },
					glm::vec3{ 0.0f, 0.0f, 0.0f },
					glm::vec3{ 1.0f, 1.0f, 1.0f }
				)
			);
		}
		cube_3d->MakeInstanced(instancedTRSs);
	}

	void Update() override
	{
		glm::vec3 pos_3d{ 0.0f, 0.0f, 0.0f };
		glm::quat rot_3d = glm::quat(glm::vec3{ glm::radians(0.0f), glm::radians(0.0f), glm::radians(0.0f) });
		glm::vec3 scale_3d{ 1.0f, 1.0f, 1.0f };
		glm::vec3 scale_model{ 1.01f, 1.01f, 1.01f };

		for (size_t i = 0; i < instancedTRSs.size(); i++)
		{
			float row = i % 20;
			float col = i / 20;
			instancedTRSs[i] *= glm::mat4_cast(glm::quat(glm::vec3{ glm::radians(seri::Random::Range(0.5f, 2.0f)), glm::radians(seri::Random::Range(0.5f, 2.0f)), glm::radians(0.0f) }));
		}
		cube_3d->UpdateInstanced(instancedTRSs);

		seri::RenderingManager::Begin(seri::Graphic::GetCameraPerspective());
		seri::RenderCommand cubesRenderCommand{};
		cubesRenderCommand.name = "cubes_instanced";
		cubesRenderCommand.camera = seri::Graphic::GetCameraPerspective();
		cubesRenderCommand.material = materialInstanced;
		cubesRenderCommand.vao = cube_3d->GetVao();
		cubesRenderCommand.desc.cullFaceEnabled = false;
		cubesRenderCommand.drawMode = seri::DrawMode::elements_instanced;
		cubesRenderCommand.instanceCount = static_cast<uint32_t>(instancedTRSs.size());
		seri::RenderingManager::Submit(cubesRenderCommand);
		seri::RenderingManager::End();

		//seri::Graphic::DrawInstanced(cube_3d, instancedTRSs, materialInstanced, seri::Graphic::GetCameraPerspective());

		if (model_0)
		{
			model_0->UpdateAnimations();
			//seri::Graphic::DrawModel(model_0, seri::Util::GetTRS(pos_3d, rot_3d, scale_model), materialSkinned, seri::Graphic::GetCameraPerspective());

			seri::RenderingManager::Begin(seri::Graphic::GetCameraPerspective());

			for (size_t m = 0; m < model_0->meshes.size(); m++)
			{
				const auto& mesh = model_0->meshes[m];
				const auto& materialSkinned = materialsSkinned[m];
				materialSkinned->SetFloat4("u_color", glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });
				materialSkinned->SetFloat3("u_view_pos", seri::Graphic::GetCameraPerspective()->GetCameraProperties().position);
				materialSkinned->SetFloat3("u_light_dir", glm::vec3{ 0.0f, 0.0f, -1.0f });
				materialSkinned->SetFloat4("u_light_color", glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });

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
						materialSkinned->SetMat4(loc.c_str(), transform);
					}
				}

				seri::RenderCommand meshRenderCommand{};
				meshRenderCommand.name = "model_" + std::to_string(m);
				meshRenderCommand.camera = seri::Graphic::GetCameraPerspective();
				meshRenderCommand.material = materialSkinned;
				meshRenderCommand.vao = mesh->GetVao();
				meshRenderCommand.trs = seri::Util::GetTRS(pos_3d, rot_3d, scale_model) * mesh->transformation;
				seri::RenderingManager::Submit(meshRenderCommand);
			}

			seri::RenderingManager::End();
		}

		//seri::Graphic::Draw(font_mesh, seri::Util::GetIdentityMatrix(), materialFont, seri::Graphic::GetCameraOrtho());

		//seri::Graphic::Draw(quad_2d, seri::Util::GetIdentityMatrix(), materialGrid, seri::Graphic::GetCameraPerspective());

		//udpSocketServer->Listen(1);
		//udpSocketClient->SendToServer();
	}

	void Destroy() override
	{
	}

private:
	seri::Light light{};

	std::unique_ptr<seri::Mesh> quad_2d;
	std::unique_ptr<seri::Mesh> quad_3d;
	std::unique_ptr<seri::Mesh> cube_3d;
	std::unique_ptr<seri::Mesh> font_mesh;

	std::unique_ptr<seri::Model> model_0;

	std::shared_ptr<seri::Material> material;
	std::shared_ptr<seri::Material> materialInstanced;
	std::shared_ptr<seri::Material> materialGrid;
	std::shared_ptr<seri::Material> materialFont;
	std::shared_ptr<seri::Material> materialModel;
	std::vector<std::shared_ptr<seri::Material>> materialsSkinned;

	std::unique_ptr<seri::netcode::Socket> udpSocketServer;
	std::unique_ptr<seri::netcode::Socket> udpSocketClient;

	std::vector<glm::mat4> instancedTRSs;

};
