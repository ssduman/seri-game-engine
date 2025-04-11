#pragma once

#include "seri/camera/ICamera.h"
#include "seri/texture/Texture.h"
#include "seri/shader/ShaderManager.h"
#include "seri/renderer/OpenGLEngineBackend.h"
#include "seri/renderer/AuxiliaryStructsBuilder.h"

#include <memory>
#include <vector>

namespace seri
{
	class Skybox
	{
	public:
		Skybox(std::shared_ptr<ICamera> camera)
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

		Skybox(std::shared_ptr<ICamera> camera, std::vector<std::string> faces)
			: _camera(camera), _faces(std::move(faces))
		{
			Init();
		}

		~Skybox()
		{
			_engineBackend.release();
		}

		void Init()
		{
			_shader = ShaderManager::Find("skybox");

			InitMVP();
			SetDefaultPositions();
			LoadCubemap();
		}

		void Render()
		{
			_shader->use();

			Bind();

			glDepthFunc(GL_LEQUAL);
			glBindTexture(GL_TEXTURE_CUBE_MAP, _tex);

			_engineBackend.draw();

			glBindVertexArray(0);
			glDepthFunc(GL_LESS);

			Unbind();

			_shader->disuse();
		}

		void Update()
		{
			if (_camera)
			{
				ShaderManager::SetView(_shader, glm::mat4(glm::mat3(_camera->getView())));
			}
		}

	private:
		void InitMVP()
		{
			ShaderManager::SetModel(_shader, glm::mat4{ 1.0f });
			ShaderManager::SetView(_shader, glm::mat4(glm::mat3(_camera->getView())));
			ShaderManager::SetProjection(_shader, _camera->getProjection());
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

			glGenTextures(1, &_tex);
			glBindTexture(GL_TEXTURE_CUBE_MAP, _tex);

			if (!flip)
			{
				std::swap(_faces[2], _faces[3]);
			}
			Texture::SetTextureFlip(flip);

			int width, height, components;
			for (size_t i = 0; i < _faces.size(); i++)
			{
				if (auto image = Texture::LoadTexture(_faces[i], width, height, components, 0))
				{
					GLenum format = GL_RED;
					if (components == 3)
					{
						format = GL_RGB;
					}
					if (components == 4)
					{
						format = GL_RGBA;
					}
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + static_cast<GLenum>(i), 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image);
					Texture::UnloadTexture(image);
				}
				else
				{
					LOGGER(error, "texture " << _faces[i] << " could not loaded: " << stbi_failure_reason());
				}
			}

			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		}

		void Bind()
		{
			glActiveTexture(GL_TEXTURE0);
		}

		void Unbind()
		{
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		std::shared_ptr<ICamera> _camera;

		std::shared_ptr<Shader> _shader;
		OpenGLEngineBackend _engineBackend{};

		unsigned int _tex{ 0 };
		std::vector<std::string> _faces;

	};
}
