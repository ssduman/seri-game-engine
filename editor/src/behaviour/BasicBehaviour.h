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
			0,  1,  2,  2,  3,  0,
			4,  5,  6,  6,  7,  4,
			8,  9, 10, 10, 11,  8,
			12, 13,14, 14, 15, 12,
			16, 17,18, 18, 19, 16,
			20, 21,22, 22, 23, 20,
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
		std::vector<glm::vec3> colors{
			{ 1.0f, 0.0f, 0.0f },
			{ 1.0f, 0.0f, 0.0f },
			{ 1.0f, 0.0f, 0.0f },
			{ 1.0f, 0.0f, 0.0f },

			{ 0.0f, 1.0f, 0.0f },
			{ 0.0f, 1.0f, 0.0f },
			{ 0.0f, 1.0f, 0.0f },
			{ 0.0f, 1.0f, 0.0f },

			{ 0.0f, 0.0f, 1.0f },
			{ 0.0f, 0.0f, 1.0f },
			{ 0.0f, 0.0f, 1.0f },
			{ 0.0f, 0.0f, 1.0f },

			{ 1.0f, 1.0f, 0.0f },
			{ 1.0f, 1.0f, 0.0f },
			{ 1.0f, 1.0f, 0.0f },
			{ 1.0f, 1.0f, 0.0f },

			{ 1.0f, 0.0f, 1.0f },
			{ 1.0f, 0.0f, 1.0f },
			{ 1.0f, 0.0f, 1.0f },
			{ 1.0f, 0.0f, 1.0f },

			{ 0.0f, 1.0f, 1.0f },
			{ 0.0f, 1.0f, 1.0f },
			{ 0.0f, 1.0f, 1.0f },
			{ 0.0f, 1.0f, 1.0f },
		};
		float vertices[] = {
			-0.5f * mult, -0.5f * mult, +0.5f * mult,  1, 0, 0,  0, 0,
			+0.5f * mult, -0.5f * mult, +0.5f * mult,  1, 0, 0,  1, 0,
			+0.5f * mult, +0.5f * mult, +0.5f * mult,  1, 0, 0,  1, 1,
			-0.5f * mult, +0.5f * mult, +0.5f * mult,  1, 0, 0,  0, 1,

			-0.5f * mult, -0.5f * mult, -0.5f * mult,  0, 1, 0,  1, 0,
			+0.5f * mult, -0.5f * mult, -0.5f * mult,  0, 1, 0,  0, 0,
			+0.5f * mult, +0.5f * mult, -0.5f * mult,  0, 1, 0,  0, 1,
			-0.5f * mult, +0.5f * mult, -0.5f * mult,  0, 1, 0,  1, 1,

			-0.5f * mult, -0.5f * mult, -0.5f * mult,  0, 0, 1,  0, 0,
			-0.5f * mult, -0.5f * mult, +0.5f * mult,  0, 0, 1,  1, 0,
			-0.5f * mult, +0.5f * mult, +0.5f * mult,  0, 0, 1,  1, 1,
			-0.5f * mult, +0.5f * mult, -0.5f * mult,  0, 0, 1,  0, 1,

			+0.5f * mult, -0.5f * mult, -0.5f * mult,  1, 1, 0,  1, 0,
			+0.5f * mult, -0.5f * mult, +0.5f * mult,  1, 1, 0,  0, 0,
			+0.5f * mult, +0.5f * mult, +0.5f * mult,  1, 1, 0,  0, 1,
			+0.5f * mult, +0.5f * mult, -0.5f * mult,  1, 1, 0,  1, 1,

			-0.5f * mult, +0.5f * mult, +0.5f * mult,  1, 0, 1,  0, 0,
			+0.5f * mult, +0.5f * mult, +0.5f * mult,  1, 0, 1,  1, 0,
			+0.5f * mult, +0.5f * mult, -0.5f * mult,  1, 0, 1,  1, 1,
			-0.5f * mult, +0.5f * mult, -0.5f * mult,  1, 0, 1,  0, 1,

			-0.5f * mult, -0.5f * mult, +0.5f * mult,  0, 1, 1,  1, 1,
			+0.5f * mult, -0.5f * mult, +0.5f * mult,  0, 1, 1,  0, 1,
			+0.5f * mult, -0.5f * mult, -0.5f * mult,  0, 1, 1,  0, 0,
			-0.5f * mult, -0.5f * mult, -0.5f * mult,  0, 1, 1,  1, 0,
		};

		basicShader = seri::ShaderLibrary::Find("basic_2");

		basicTexture = seri::TextureBase::Create();
		basicTexture->Init(seri::TextureDesc{}, "assets/textures/passage.png");

		basicVertexBuffer_0 = seri::VertexBufferBase::Create(&positions[0], positions.size() * 3 * sizeof(float));
		basicVertexBuffer_0->AddElement(
			{ seri::LayoutLocation::position, seri::ShaderDataType::float3_type, false }
		);
		basicVertexBuffer_1 = seri::VertexBufferBase::Create(&uvs[0], uvs.size() * 2 * sizeof(float));
		basicVertexBuffer_1->AddElement(
			{ seri::LayoutLocation::uv0, seri::ShaderDataType::float2_type }
		);
		basicVertexBuffer_2 = seri::VertexBufferBase::Create(&colors[0], colors.size() * 3 * sizeof(float));
		basicVertexBuffer_2->AddElement(
			{ seri::LayoutLocation::color, seri::ShaderDataType::float3_type }
		);
		basicVertexBuffer_all = seri::VertexBufferBase::Create(&vertices[0], sizeof(vertices));
		basicVertexBuffer_all->
			AddElement({ seri::LayoutLocation::position, seri::ShaderDataType::float3_type }).
			AddElement({ seri::LayoutLocation::color, seri::ShaderDataType::float3_type }).
			AddElement({ seri::LayoutLocation::uv0, seri::ShaderDataType::float2_type }
			);
		basicVertexBuffer_all->SetLayout(
			seri::BufferLayoutDesc{
				{ seri::LayoutLocation::position, seri::ShaderDataType::float3_type },
				{ seri::LayoutLocation::color, seri::ShaderDataType::float3_type },
				{ seri::LayoutLocation::uv0, seri::ShaderDataType::float2_type }
			}
		);

		basicIndexBuffer = seri::IndexBufferBase::Create(&indices[0], indices.size());
		basicVertexArray = seri::VertexArrayBase::Create();
		basicVertexArray->SetIndexBuffer(basicIndexBuffer);
		//basicVertexArray->AddVertexBuffer(basicVertexBuffer_0);
		//basicVertexArray->AddVertexBuffer(basicVertexBuffer_1);
		//basicVertexArray->AddVertexBuffer(basicVertexBuffer_2);
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
		seri::ShaderLibrary::SetMVP(basicShader, seri::Graphic::GetCameraPerspective());
		//basicShader->Bind();
		//basicTexture->Bind();
		//basicVertexArray->Bind();
		//glDrawElements(GL_TRIANGLES, basicIndexBuffer->GetCount(), GL_UNSIGNED_INT, 0);
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
	std::shared_ptr<seri::VertexBufferBase> basicVertexBuffer_all;
	std::shared_ptr<seri::VertexArrayBase> basicVertexArray;

};
