#pragma once

#include <seri/core/Seri.h>
#include <seri/graphic/Mesh.h>
#include <seri/graphic/Model.h>
#include <seri/graphic/Material.h>
#include <seri/random/Random.h>
#include <seri/netcode/Socket.h>
#include <seri/model/ModelImporter.h>
#include <seri/behaviour/BehaviourBase.h>

namespace seri::editor
{
	class SimpleBehaviour : public seri::BehaviourBase
	{
	public:
		void Init() override
		{
			cube3d = seri::Mesh::cube_3d();
			fontMesh = std::make_shared<seri::Mesh>();
			modelSkinned = seri::ModelImporter{}.Load("assets/spiderman/source/spiderman.fbx");

			auto texture0 = seri::TextureBase::Create();
			texture0->Init(seri::TextureDesc{}, "assets/textures/passage.png");

			auto texture1 = seri::TextureBase::Create();
			texture1->Init(seri::TextureDesc{}, "assets/spiderman/textures/spiderman.png");

			materialFont = std::make_shared<seri::Material>();
			materialFont->SetShader(seri::ShaderLibrary::Find("typer"));
			materialFont->SetTexture("u_texture", seri::font::FontManager::GetPredefinedFonts()[fontIndex]->texture);

			materialInstanced = std::make_shared<seri::Material>();
			materialInstanced->SetShader(seri::ShaderLibrary::Find("entity_instanced"));
			materialInstanced->SetTexture("u_texture", texture0);

			for (size_t i = 0; modelSkinned && i < modelSkinned->meshes.size(); i++)
			{
				auto materialSkinned = std::make_shared<seri::Material>();
				materialSkinned->SetShader(seri::ShaderLibrary::Find("entity_skinned"));
				materialSkinned->SetTexture("u_texture", texture1);
				materialsSkinned.emplace_back(materialSkinned);
			}

			for (unsigned int i = 0; i < instanceCount; i++)
			{
				instancedTRSs.push_back(seri::Util::GetTRS({ 1.0f + i % 20, i / 20, -1.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }));
			}
			cube3d->MakeInstanced(instancedTRSs);

			udpSocketServer = std::make_unique<seri::netcode::Socket>(seri::netcode::SocketType::udp);
			udpSocketServer->Bind({ "localhost", 5200 });
			udpSocketClient = std::make_unique<seri::netcode::Socket>(seri::netcode::SocketType::udp);
			udpSocketClient->Connect({ "127.0.0.1", 5200 });
		}

		void Update() override
		{
			for (size_t i = 0; i < instancedTRSs.size(); i++)
			{
				instancedTRSs[i] *= glm::mat4_cast(seri::Util::ToQuaternion({ seri::Random::Range(0.5f, 2.0f), seri::Random::Range(0.5f, 2.0f), 0.0f }));
			}
			cube3d->UpdateInstanced(instancedTRSs);
			seri::Graphic::DrawInstanced(cube3d, materialInstanced, instancedTRSs);

			if (modelSkinned)
			{
				modelSkinned->UpdateAnimations(seri::WindowManager::GetTime());

				for (size_t m = 0; m < modelSkinned->meshes.size(); m++)
				{
					const auto& mesh = modelSkinned->meshes[m];
					const auto& materialSkinned = materialsSkinned[m];
					seri::Graphic::Draw(mesh, materialSkinned, seri::Util::GetTRS({ -1.5f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }));
				}
			}

			seri::RenderingStats renderingStats = seri::RenderingManager::GetRenderingStats();
			std::string statsStr = fmt::format("draw calls: {}, tri: {}, fps: {}", renderingStats.drawCalls, renderingStats.triangles, seri::TimeWrapper::GetFPS());
			seri::font::FontManager::MakeText(fontMesh, fontIndex, statsStr, -seri::WindowManager::GetWidth() / 2 + 20, -seri::WindowManager::GetHeight() / 2 + 20);
			seri::Graphic::Draw(fontMesh, materialFont, seri::Util::GetIdentityMatrix(), seri::PassType::ui);
		}

		void Destroy() override
		{
		}

	private:
		int fontIndex = 2;
		int instanceCount = 128;

		std::shared_ptr<seri::Mesh> cube3d;
		std::shared_ptr<seri::Mesh> fontMesh;
		std::shared_ptr<seri::Model> modelSkinned;

		std::shared_ptr<seri::Material> materialFont;
		std::shared_ptr<seri::Material> materialInstanced;
		std::vector<std::shared_ptr<seri::Material>> materialsSkinned;

		std::vector<glm::mat4> instancedTRSs;

		std::unique_ptr<seri::netcode::Socket> udpSocketServer;
		std::unique_ptr<seri::netcode::Socket> udpSocketClient;

	};
}
