#pragma once

#include "seri/camera/CameraBase.h"
#include "seri/shader/ShaderLibrary.h"
#include "seri/texture/TextureBase.h"
#include "seri/rendering/RenderingManagerFactory.h"
#include "seri/renderer/RendererBackendOpenGL.h"
#include "seri/renderer/AuxiliaryStructsBuilder.h"

#include <memory>
#include <vector>

namespace seri
{
	class Skybox
	{
	public:
		Skybox(std::shared_ptr<CameraBase> camera)
			: _camera(camera)
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

		Skybox(std::shared_ptr<CameraBase> camera, std::vector<std::string> faces)
			: _camera(camera), _faces(std::move(faces))
		{
			Init();
		}

		~Skybox()
		{
			_engineBackend.Release();
		}

		void Init()
		{
			_shader = ShaderLibrary::Find("skybox");
			_texture = TextureBase::Create();

			InitMVP();
			SetDefaultPositions();
			LoadCubemap();
		}

		void Render()
		{
			_shader->Bind();
			_texture->Bind();

			DepthFuncType oldDepthFuncType = RenderingManagerFactory::Instance()->SetDepthFunc(DepthFuncType::l_equal);

			_engineBackend.Draw();

			RenderingManagerFactory::Instance()->SetDepthFunc(oldDepthFuncType);

			_texture->Unbind();
			_shader->Unbind();
		}

		void Update()
		{
			if (_camera)
			{
				ShaderLibrary::SetView(_shader, glm::mat4(glm::mat3(_camera->GetView())));
			}
		}

	private:
		void InitMVP()
		{
			ShaderLibrary::SetModel(_shader, glm::mat4{ 1.0f });
			ShaderLibrary::SetView(_shader, glm::mat4(glm::mat3(_camera->GetView())));
			ShaderLibrary::SetProjection(_shader, _camera->GetProjection());
		}

		void SetDefaultPositions()
		{
			const std::vector<glm::vec3> positions = {
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

			aux::DataBufferBuilder dataBufferBuilder;
			aux::AttributeBuilder attributeBuilder;

			auto dataBuffer = dataBufferBuilder
				.setTarget(aux::Target::vbo)
				.setSize(aux::count(positions) * 12)
				.setData(aux::data(positions))
				.setUsage(aux::Usage::static_draw)
				.build();
			_engineBackend.setDataBuffer(dataBuffer);

			auto attribute = attributeBuilder
				.setIndex(aux::Index::position)
				.setSize(aux::length(positions))
				.setPointer(nullptr)
				.build();
			_engineBackend.setAttribute(attribute);

			_engineBackend.setEnable(aux::Index::position);
			_engineBackend.setDrawCount(aux::count(positions));
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

		std::shared_ptr<CameraBase> _camera;
		std::shared_ptr<ShaderBase> _shader;
		std::shared_ptr<TextureBase> _texture;

		RendererBackendOpenGL _engineBackend{};

		std::vector<std::string> _faces;

	};
}
