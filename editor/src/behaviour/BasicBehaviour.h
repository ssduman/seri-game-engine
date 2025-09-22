#pragma once

#include <seri/core/Seri.h>
#include <seri/behaviour/BehaviourBase.h>

class BasicBehaviour : public seri::BehaviourBase
{
public:
	void Init() override
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
		basicMaterial->shader = basicShader;
		basicMaterial->SetTexture("u_texture", basicTexture);
	}

	void Update() override
	{
		seri::RenderingManager::Begin(seri::Graphic::GetCameraPerspective());

		seri::RenderCommand renderCommand{};
		renderCommand.name = "basic_cube";
		renderCommand.camera = seri::Graphic::GetCameraPerspective();
		renderCommand.material = basicMaterial;
		renderCommand.vao = basicVertexArray;
		renderCommand.trs = seri::Util::GetTRS(
			{ 0.0f, 0.0f, 0.0f },
			{ 0.0f, 45.0f, 0.0f },
			{ 1.0f, 1.0f, 1.0f }
		);

		seri::RenderingManager::Submit(renderCommand);

		seri::RenderingManager::End();
	}

	void Destroy() override
	{
	}

private:
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

};
