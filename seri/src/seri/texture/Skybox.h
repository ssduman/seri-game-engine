#pragma once

#include "seri/core/Object.h"
#include "seri/camera/ICamera.h"
#include "seri/texture/Texture.h"
#include "seri/renderer/OpenGLEngineBackend.h"
#include "seri/renderer/AuxiliaryStructsBuilder.h"

#include <memory>
#include <vector>

class Skybox : public Object
{
public:
	Skybox(std::shared_ptr<ICamera> camera, std::vector<std::string> faces)
		: _camera(camera), _faces(std::move(faces))
	{}

	~Skybox() override
	{
		_engineBackend.release();
	}

	void init() override
	{
		initMVP();
		setDefaultPositions();
		loadCubemap();
	}

	void render() override
	{
		_shader.use();

		bind();

		glDepthFunc(GL_LEQUAL);
		glBindTexture(GL_TEXTURE_CUBE_MAP, _tex);

		_engineBackend.draw();

		glBindVertexArray(0);
		glDepthFunc(GL_LESS);

		unbind();

		_shader.disuse();
	}

	void update() override
	{
		if (_camera)
		{
			ShaderManager::GetInstance().setView(_shader, glm::mat4(glm::mat3(_camera->getView())));
		}
	}

	Shader& getShader()
	{
		return _shader;
	}

	OpenGLEngineBackend& getDrawer()
	{
		return _engineBackend;
	}

private:
	void initMVP()
	{
		ShaderManager::GetInstance().setModel(_shader, glm::mat4{ 1.0f });
		ShaderManager::GetInstance().setView(_shader, glm::mat4(glm::mat3(_camera->getView())));
		ShaderManager::GetInstance().setProjection(_shader, _camera->getProjection());
	}

	void setDefaultPositions()
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

	void loadCubemap(bool flip = false)
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
		Texture::setTextureFlip(flip);

		int width, height, components;
		for (size_t i = 0; i < _faces.size(); i++)
		{
			if (auto image = Texture::loadTexture(_faces[i], width, height, components, 0))
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
				Texture::unloadTexture(image);
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

	void bind()
	{
		glActiveTexture(GL_TEXTURE0);
	}

	void unbind()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	std::shared_ptr<ICamera> _camera;

	Shader _shader;
	OpenGLEngineBackend _engineBackend{};

	unsigned int _tex{ 0 };
	std::vector<std::string> _faces;

};