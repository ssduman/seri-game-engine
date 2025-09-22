#pragma once

#include "seri/util/Util.h"
#include "seri/camera/CameraBase.h"
#include "seri/graphic/Graphic.h"
#include "seri/graphic/Material.h"
#include "seri/texture/TextureBase.h"
#include "seri/shader/ShaderLibrary.h"
#include "seri/rendering/RenderingManager.h"

namespace seri
{
	class Skybox
	{
	public:
		Skybox()
		{
			_faces = {
				"assets/textures/skybox/right.jpg",
				"assets/textures/skybox/left.jpg",
				"assets/textures/skybox/bottom.jpg",
				"assets/textures/skybox/top.jpg",
				"assets/textures/skybox/front.jpg",
				"assets/textures/skybox/back.jpg",
			};

			Init();
		}

		Skybox(std::vector<std::string> faces) :  _faces(std::move(faces))
		{
			Init();
		}

		void Init()
		{
			_texture = TextureBase::Create();
			_material = std::make_shared<Material>();
			_material->shader = ShaderLibrary::Find("skybox");;
			_material->SetTexture("u_skybox", _texture);

			SetDefaultPositions();
			LoadCubemap();
		}

		void Update()
		{
			seri::RenderingManager::Begin(seri::Graphic::GetCameraPerspective());

			_material->SetMat4("u_view_skybox", glm::mat4(glm::mat3(seri::Graphic::GetCameraPerspective()->GetView())));

			seri::RenderCommand renderCommand_skybox{};
			renderCommand_skybox.name = "skybox";
			renderCommand_skybox.desc.depthFunc = DepthFunc::l_equal;
			renderCommand_skybox.camera = seri::Graphic::GetCameraPerspective();
			renderCommand_skybox.drawMode = DrawMode::arrays;
			renderCommand_skybox.material = _material;
			renderCommand_skybox.vao = _vertexArray;
			renderCommand_skybox.count = static_cast<uint32_t>(_positions.size());
			seri::RenderingManager::Submit(renderCommand_skybox);

			seri::RenderCommand renderCommand_restore{};
			renderCommand_restore.name = "skybox_restore";
			renderCommand_restore.noop = true;
			seri::RenderingManager::Submit(renderCommand_restore);

			seri::RenderingManager::End();
		}

	private:
		void SetDefaultPositions()
		{
			_positions = {
				{ -1.0f, 1.0f, -1.0f },
				{ -1.0f, -1.0f, -1.0f },
				{ 1.0f, -1.0f, -1.0f },
				{ 1.0f, -1.0f, -1.0f },
				{ 1.0f, 1.0f, -1.0f },
				{ -1.0f, 1.0f, -1.0f },

				{ -1.0f, -1.0f, 1.0f },
				{ -1.0f, -1.0f, -1.0f },
				{ -1.0f, 1.0f, -1.0f },
				{ -1.0f, 1.0f, -1.0f },
				{ -1.0f, 1.0f, 1.0f },
				{ -1.0f, -1.0f, 1.0f },

				{ 1.0f, -1.0f, -1.0f },
				{ 1.0f, -1.0f, 1.0f },
				{ 1.0f, 1.0f, 1.0f },
				{ 1.0f, 1.0f, 1.0f },
				{ 1.0f, 1.0f, -1.0f },
				{ 1.0f, -1.0f, -1.0f },

				{ -1.0f, -1.0f, 1.0f },
				{ -1.0f, 1.0f, 1.0f },
				{ 1.0f, 1.0f, 1.0f },
				{ 1.0f, 1.0f, 1.0f },
				{ 1.0f, -1.0f, 1.0f },
				{ -1.0f, -1.0f, 1.0f },

				{ -1.0f, 1.0f, -1.0f },
				{ 1.0f, 1.0f, -1.0f },
				{ 1.0f, 1.0f, 1.0f },
				{ 1.0f, 1.0f, 1.0f },
				{ -1.0f, 1.0f, 1.0f },
				{ -1.0f, 1.0f, -1.0f },

				{ -1.0f, -1.0f, -1.0f },
				{ -1.0f, -1.0f, 1.0f },
				{ 1.0f, -1.0f, -1.0f },
				{ 1.0f, -1.0f, -1.0f },
				{ -1.0f, -1.0f, 1.0f },
				{ 1.0f, -1.0f, 1.0f },
			};

			auto& vertexBuffer = seri::VertexBufferBase::Create(&_positions[0], _positions.size() * 3 * sizeof(float));
			vertexBuffer->AddElement(
				{ seri::LayoutLocation::vertex, seri::ShaderDataType::float3_type, false }
			);

			_vertexArray = seri::VertexArrayBase::Create();
			_vertexArray->AddVertexBuffer(vertexBuffer);
		}

		void LoadCubemap(bool flip = false)
		{
			if (_faces.empty() || _faces.size() != 6)
			{
				LOGGER(error, "there should be exactly 6 textures for skybox");
				return;
			}

			if (!flip)
			{
				std::swap(_faces[2], _faces[3]);
			}

			TextureDesc desc{};
			desc.flip = flip;
			desc.target = TextureTarget::cube_map;
			desc.wrap_s = TextureWrap::clamp_to_edge;
			desc.wrap_t = TextureWrap::clamp_to_edge;
			desc.wrap_r = TextureWrap::clamp_to_edge;
			desc.magFilter = TextureMagFilter::linear;
			desc.minFilter = TextureMinFilter::linear;
			_texture->Init(desc);
			_texture->LoadCubeMap(_faces);
		}

		std::vector<std::string> _faces;
		std::vector<glm::vec3> _positions;

		std::shared_ptr<Material> _material;
		std::shared_ptr<TextureBase> _texture;
		std::shared_ptr<seri::VertexArrayBase> _vertexArray;

	};
}
