#pragma once

#include <seri/core/Seri.h>
#include <seri/graphic/Mesh.h>
#include <seri/graphic/Model.h>
#include <seri/graphic/Material.h>
#include <seri/random/Random.h>
#include <seri/netcode/Socket.h>

namespace seri::editor
{
	class SandboxLayer : public seri::LayerBase
	{
	public:
		SandboxLayer() : LayerBase("SandboxLayer")
		{
			if (runBasic)
			{
				InitBasic();
			}
			if (runSimple)
			{
				InitSimple();
			}
		}

		~SandboxLayer() override = default;

		void OnUpdate() override
		{
			if (runBasic)
			{
				UpdateBasic();
			}
			if (runSimple)
			{
				UpdateSimple();
			}
		}

	private:
		void InitBasic()
		{
			float mult = 0.4f;
			std::vector<uint32_t> indices{
				0,  1,   2,  2,  3,  0,
				4,  5,   6,  6,  7,  4,
				8,  9,  10, 10, 11,  8,
				12, 13, 14, 14, 15, 12,
				16, 17, 18, 18, 19, 16,
				20, 21, 22, 22, 23, 20,
			};
			std::vector<glm::vec3> positions{
				{-0.5f * mult, -0.5f * mult, +0.5f * mult },
				{+0.5f * mult, -0.5f * mult, +0.5f * mult },
				{+0.5f * mult, +0.5f * mult, +0.5f * mult },
				{-0.5f * mult, +0.5f * mult, +0.5f * mult },
				{-0.5f * mult, -0.5f * mult, -0.5f * mult },
				{+0.5f * mult, -0.5f * mult, -0.5f * mult },
				{+0.5f * mult, +0.5f * mult, -0.5f * mult },
				{-0.5f * mult, +0.5f * mult, -0.5f * mult },
				{-0.5f * mult, -0.5f * mult, -0.5f * mult },
				{-0.5f * mult, -0.5f * mult, +0.5f * mult },
				{-0.5f * mult, +0.5f * mult, +0.5f * mult },
				{-0.5f * mult, +0.5f * mult, -0.5f * mult },
				{+0.5f * mult, -0.5f * mult, -0.5f * mult },
				{+0.5f * mult, -0.5f * mult, +0.5f * mult },
				{+0.5f * mult, +0.5f * mult, +0.5f * mult },
				{+0.5f * mult, +0.5f * mult, -0.5f * mult },
				{-0.5f * mult, +0.5f * mult, +0.5f * mult },
				{+0.5f * mult, +0.5f * mult, +0.5f * mult },
				{+0.5f * mult, +0.5f * mult, -0.5f * mult },
				{-0.5f * mult, +0.5f * mult, -0.5f * mult },
				{-0.5f * mult, -0.5f * mult, +0.5f * mult },
				{+0.5f * mult, -0.5f * mult, +0.5f * mult },
				{+0.5f * mult, -0.5f * mult, -0.5f * mult },
				{-0.5f * mult, -0.5f * mult, -0.5f * mult },
			};
			std::vector<glm::vec2> uvs{
				{ 0.0f, 0.0f },
				{ 1.0f, 0.0f },
				{ 1.0f, 1.0f },
				{ 0.0f, 1.0f },

				{ 1.0f, 0.0f },
				{ 0.0f, 0.0f },
				{ 0.0f, 1.0f },
				{ 1.0f, 1.0f },

				{ 0.0f, 0.0f },
				{ 1.0f, 0.0f },
				{ 1.0f, 1.0f },
				{ 0.0f, 1.0f },

				{ 1.0f, 0.0f },
				{ 0.0f, 0.0f },
				{ 0.0f, 1.0f },
				{ 1.0f, 1.0f },

				{ 0.0f, 0.0f },
				{ 1.0f, 0.0f },
				{ 1.0f, 1.0f },
				{ 0.0f, 1.0f },

				{ 1.0f, 1.0f },
				{ 0.0f, 1.0f },
				{ 0.0f, 0.0f },
				{ 1.0f, 0.0f },
			};
			std::vector<glm::vec4> colors{
				{ 1.0f, 0.0f, 0.0f, 1.0f },
				{ 1.0f, 0.0f, 0.0f, 1.0f },
				{ 1.0f, 0.0f, 0.0f, 1.0f },
				{ 1.0f, 0.0f, 0.0f, 1.0f },

				{ 0.0f, 1.0f, 0.0f, 1.0f },
				{ 0.0f, 1.0f, 0.0f, 1.0f },
				{ 0.0f, 1.0f, 0.0f, 1.0f },
				{ 0.0f, 1.0f, 0.0f, 1.0f },

				{ 0.0f, 0.0f, 1.0f, 1.0f },
				{ 0.0f, 0.0f, 1.0f, 1.0f },
				{ 0.0f, 0.0f, 1.0f, 1.0f },
				{ 0.0f, 0.0f, 1.0f, 1.0f },

				{ 1.0f, 1.0f, 0.0f, 1.0f },
				{ 1.0f, 1.0f, 0.0f, 1.0f },
				{ 1.0f, 1.0f, 0.0f, 1.0f },
				{ 1.0f, 1.0f, 0.0f, 1.0f },

				{ 1.0f, 0.0f, 1.0f, 1.0f },
				{ 1.0f, 0.0f, 1.0f, 1.0f },
				{ 1.0f, 0.0f, 1.0f, 1.0f },
				{ 1.0f, 0.0f, 1.0f, 1.0f },

				{ 0.0f, 1.0f, 1.0f, 1.0f },
				{ 0.0f, 1.0f, 1.0f, 1.0f },
				{ 0.0f, 1.0f, 1.0f, 1.0f },
				{ 0.0f, 1.0f, 1.0f, 1.0f },
			};
			std::vector<glm::vec3> normals{
				{  0.0f,  0.0f,  1.0f },
				{  0.0f,  0.0f,  1.0f },
				{  0.0f,  0.0f,  1.0f },
				{  0.0f,  0.0f,  1.0f },

				{  0.0f,  0.0f, -1.0f },
				{  0.0f,  0.0f, -1.0f },
				{  0.0f,  0.0f, -1.0f },
				{  0.0f,  0.0f, -1.0f },

				{ -1.0f,  0.0f,  0.0f },
				{ -1.0f,  0.0f,  0.0f },
				{ -1.0f,  0.0f,  0.0f },
				{ -1.0f,  0.0f,  0.0f },

				{ +1.0f,  0.0f,  0.0f },
				{ +1.0f,  0.0f,  0.0f },
				{ +1.0f,  0.0f,  0.0f },
				{ +1.0f,  0.0f,  0.0f },

				{  0.0f, +1.0f,  0.0f },
				{  0.0f, +1.0f,  0.0f },
				{  0.0f, +1.0f,  0.0f },
				{  0.0f, +1.0f,  0.0f },

				{  0.0f, -1.0f,  0.0f },
				{  0.0f, -1.0f,  0.0f },
				{  0.0f, -1.0f,  0.0f },
				{  0.0f, -1.0f,  0.0f },
			};
			float verticesAll[] = {
				// positions                               // colors                // uvs
				-0.5f * mult, -0.5f * mult, +0.5f * mult,  1.0f, 0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
				+0.5f * mult, -0.5f * mult, +0.5f * mult,  1.0f, 0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
				+0.5f * mult, +0.5f * mult, +0.5f * mult,  1.0f, 0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
				-0.5f * mult, +0.5f * mult, +0.5f * mult,  1.0f, 0.0f, 0.0f, 1.0f,  0.0f, 1.0f,

				-0.5f * mult, -0.5f * mult, -0.5f * mult,  0.0f, 1.0f, 0.0f, 1.0f,  1.0f, 0.0f,
				+0.5f * mult, -0.5f * mult, -0.5f * mult,  0.0f, 1.0f, 0.0f, 1.0f,  0.0f, 0.0f,
				+0.5f * mult, +0.5f * mult, -0.5f * mult,  0.0f, 1.0f, 0.0f, 1.0f,  0.0f, 1.0f,
				-0.5f * mult, +0.5f * mult, -0.5f * mult,  0.0f, 1.0f, 0.0f, 1.0f,  1.0f, 1.0f,

				-0.5f * mult, -0.5f * mult, -0.5f * mult,  0.0f, 0.0f, 1.0f, 1.0f,  0.0f, 0.0f,
				-0.5f * mult, -0.5f * mult, +0.5f * mult,  0.0f, 0.0f, 1.0f, 1.0f,  1.0f, 0.0f,
				-0.5f * mult, +0.5f * mult, +0.5f * mult,  0.0f, 0.0f, 1.0f, 1.0f,  1.0f, 1.0f,
				-0.5f * mult, +0.5f * mult, -0.5f * mult,  0.0f, 0.0f, 1.0f, 1.0f,  0.0f, 1.0f,

				+0.5f * mult, -0.5f * mult, -0.5f * mult,  1.0f, 1.0f, 0.0f, 1.0f,  1.0f, 0.0f,
				+0.5f * mult, -0.5f * mult, +0.5f * mult,  1.0f, 1.0f, 0.0f, 1.0f,  0.0f, 0.0f,
				+0.5f * mult, +0.5f * mult, +0.5f * mult,  1.0f, 1.0f, 0.0f, 1.0f,  0.0f, 1.0f,
				+0.5f * mult, +0.5f * mult, -0.5f * mult,  1.0f, 1.0f, 0.0f, 1.0f,  1.0f, 1.0f,

				-0.5f * mult, +0.5f * mult, +0.5f * mult,  1.0f, 0.0f, 1.0f, 1.0f,  0.0f, 0.0f,
				+0.5f * mult, +0.5f * mult, +0.5f * mult,  1.0f, 0.0f, 1.0f, 1.0f,  1.0f, 0.0f,
				+0.5f * mult, +0.5f * mult, -0.5f * mult,  1.0f, 0.0f, 1.0f, 1.0f,  1.0f, 1.0f,
				-0.5f * mult, +0.5f * mult, -0.5f * mult,  1.0f, 0.0f, 1.0f, 1.0f,  0.0f, 1.0f,

				-0.5f * mult, -0.5f * mult, +0.5f * mult,  0.0f, 1.0f, 1.0f, 1.0f,  1.0f, 1.0f,
				+0.5f * mult, -0.5f * mult, +0.5f * mult,  0.0f, 1.0f, 1.0f, 1.0f,  0.0f, 1.0f,
				+0.5f * mult, -0.5f * mult, -0.5f * mult,  0.0f, 1.0f, 1.0f, 1.0f,  0.0f, 0.0f,
				-0.5f * mult, -0.5f * mult, -0.5f * mult,  0.0f, 1.0f, 1.0f, 1.0f,  1.0f, 0.0f,
			};

			basicShader = seri::ShaderLibrary::Find("basic_2");

			basicTexture = seri::TextureBase::Create();
			basicTexture->Init(seri::TextureDesc{}, "assets/textures/passage.png");

			basicVertexBuffer_0 = seri::VertexBufferBase::Create(positions);
			basicVertexBuffer_0->AddElement(
				{ seri::LayoutLocation::vertex }
			);

			basicVertexBuffer_1 = seri::VertexBufferBase::Create(uvs);
			basicVertexBuffer_1->AddElement(
				{ seri::LayoutLocation::uv0 }
			);

			basicVertexBuffer_2 = seri::VertexBufferBase::Create(colors);
			basicVertexBuffer_2->AddElement(
				{ seri::LayoutLocation::color }
			);

			basicVertexBuffer_3 = seri::VertexBufferBase::Create(normals);
			basicVertexBuffer_3->AddElement(
				{ seri::LayoutLocation::normal }
			);

			basicVertexBuffer_all = seri::VertexBufferBase::Create(&verticesAll[0], sizeof(verticesAll));
			basicVertexBuffer_all->SetLayout(
				seri::BufferLayoutDesc{
					{ seri::LayoutLocation::vertex },
					{ seri::LayoutLocation::color },
					{ seri::LayoutLocation::uv0 }
				}
			);

			basicIndexBuffer = seri::IndexBufferBase::Create(indices);
			basicVertexArray = seri::VertexArrayBase::Create();
			basicVertexArray->SetIndexBuffer(basicIndexBuffer);
			//basicVertexArray->AddVertexBuffer(basicVertexBuffer_0);
			//basicVertexArray->AddVertexBuffer(basicVertexBuffer_1);
			//basicVertexArray->AddVertexBuffer(basicVertexBuffer_2);
			//basicVertexArray->AddVertexBuffer(basicVertexBuffer_3);
			basicVertexArray->AddVertexBuffer(basicVertexBuffer_all);

			basicMaterial = std::make_shared<seri::Material>();
			basicMaterial->SetShader(basicShader);
			basicMaterial->SetTexture("u_texture", basicTexture);
		}

		void InitSimple()
		{
			cube3d = seri::Mesh::cube_3d();
			fontMesh = std::make_shared<seri::Mesh>();

			auto texture0 = seri::TextureBase::Create();
			texture0->Init(seri::TextureDesc{}, "assets/textures/passage.png");

			materialFont = std::make_shared<seri::Material>();
			materialFont->SetShader(seri::ShaderLibrary::Find("text"));

			materialInstanced = std::make_shared<seri::Material>();
			materialInstanced->SetShader(seri::ShaderLibrary::Find("entity_instanced"));
			materialInstanced->SetTexture("u_texture", texture0);

			for (int i = 0; i < instanceCount; i++)
			{
				instancedTRSs.push_back(seri::Util::GetTRS({ 1.0f + i % 20, i / 20, -1.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }));
			}

			//udpSocketServer = std::make_unique<seri::netcode::Socket>(seri::netcode::SocketType::udp);
			//udpSocketServer->Bind({ "localhost", 5200 });
			//udpSocketClient = std::make_unique<seri::netcode::Socket>(seri::netcode::SocketType::udp);
			//udpSocketClient->Connect({ "127.0.0.1", 5200 });
		}

		void UpdateBasic()
		{
			seri::RenderCommand renderCommand{};
			renderCommand.name = "basic_cube";
			renderCommand.camera = seri::Graphic::GetActiveCamera();
			renderCommand.material = basicMaterial;
			renderCommand.vao = basicVertexArray;
			renderCommand.rt = seri::RenderingManager::GetEditorRT();
			renderCommand.model = seri::Util::GetTRS(
				{ 0.0f, 0.0f, 0.0f },
				{ 0.0f, 45.0f, 0.0f },
				{ 1.0f, 1.0f, 1.0f }
			);

			seri::RenderingManager::Submit(renderCommand);
		}

		void UpdateSimple()
		{
			auto dt = seri::TimeWrapper::GetDeltaTime();

			const bool drawInstanced = false;
			if (drawInstanced)
			{
				for (size_t i = 0; i < instancedTRSs.size(); i++)
				{
					instancedTRSs[i] *= glm::mat4_cast(seri::Util::ToQuaternion({ seri::Random::Range(1.0f, 90.0f) * dt, seri::Random::Range(1.0f, 90.0f) * dt, 0.0f }));
				}
				seri::Graphic::DrawInstanced(cube3d, materialInstanced, instancedTRSs);
			}

			const bool drawText = true;
			if (drawText)
			{
				std::shared_ptr<seri::font::Font> font = seri::font::FontManager::GetDefaultFont();
				if (font)
				{
					seri::RenderingStats renderingStats = seri::RenderingManager::GetRenderingStats();
					std::string statsStr = fmt::format("draw calls: {}, tri: {}, fps: {}", renderingStats.drawCalls, renderingStats.triangles, seri::TimeWrapper::GetFPS());

					seri::font::TextDesc textDesc{};
					textDesc.fontSize = 36.0f;
					textDesc.alignH = seri::font::TextAlignH::left;
					textDesc.alignV = seri::font::TextAlignV::bottom;

					seri::font::TextMesh::BuildTextMesh(*fontMesh, *font, statsStr, textDesc);

					if (fontMesh->GetVao())
					{
						auto editorRT = seri::RenderingManager::GetEditorRT();
						glm::vec3 textPos{
							-static_cast<float>(editorRT->GetWidth()) / 2.0f + 20.0f,
							-static_cast<float>(editorRT->GetHeight()) / 2.0f + 20.0f,
							0.0f
						};

						materialFont->SetTexture(seri::literals::kUniformTextTexture, font->GetAtlas());
						materialFont->SetFloat4(seri::literals::kUniformTextColor, glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });

						seri::Graphic::Draw(fontMesh, materialFont, glm::translate(glm::mat4{ 1.0f }, textPos), seri::PassType::ui);
					}
				}
			}
		}

		bool runBasic{ false };
		bool runSimple{ true };

		std::shared_ptr<seri::Material> basicMaterial;
		std::shared_ptr<seri::ShaderBase> basicShader;
		std::shared_ptr<seri::TextureBase> basicTexture;
		std::shared_ptr<seri::IndexBufferBase> basicIndexBuffer;
		std::shared_ptr<seri::VertexBufferBase> basicVertexBuffer_0;
		std::shared_ptr<seri::VertexBufferBase> basicVertexBuffer_1;
		std::shared_ptr<seri::VertexBufferBase> basicVertexBuffer_2;
		std::shared_ptr<seri::VertexBufferBase> basicVertexBuffer_3;
		std::shared_ptr<seri::VertexBufferBase> basicVertexBuffer_all;
		std::shared_ptr<seri::VertexArrayBase> basicVertexArray;

		int instanceCount = 128;

		std::shared_ptr<seri::Mesh> cube3d;
		std::shared_ptr<seri::Mesh> fontMesh;

		std::shared_ptr<seri::Material> materialFont;
		std::shared_ptr<seri::Material> materialInstanced;

		std::vector<glm::mat4> instancedTRSs;

		std::unique_ptr<seri::netcode::Socket> udpSocketServer;
		std::unique_ptr<seri::netcode::Socket> udpSocketClient;

	};
}
