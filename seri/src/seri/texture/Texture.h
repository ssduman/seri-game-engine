#pragma once

#include "seri/logging/Logger.h"
#include "seri/renderer/AuxiliaryStructs.h"

#include <glad/gl.h>
#include <stb_image.h>

class Texture
{
public:
	Texture() = default;

	Texture(Texture& other) = default;

	Texture(Texture&& other) = default;

	Texture& operator=(Texture& other) = default;

	Texture& operator=(Texture&& other) = default;

	~Texture()
	{
		unbind();
		del();

		LOGGER(info, "texture destroyed");
	}

	void init(const std::string& texturePath)
	{
		int width, height, components;
		if (auto image = loadTexture(texturePath, width, height, components, 0))
		{
			init(image, width, height, components);
		}
		else
		{
			LOGGER(error, "init texture '" << texturePath << "' failed: " << stbi_failure_reason());
		}
	}

	void init(const void* data, unsigned int size)
	{
		int width, height, components;
		if (auto image = loadTexture(data, size, width, height, components, 0))
		{
			init(image, width, height, components);
		}
		else
		{
			LOGGER(error, "init texture from buffer with size " << size << " failed: " << stbi_failure_reason());
		}
	}

	void bind()
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _tex);
	}

	void unbind()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	unsigned int GetTex()
	{
		return _tex;
	}

	static unsigned char* loadTexture(const std::string& texturePath, int& width, int& height, int& components, int reqComp)
	{
		return stbi_load(texturePath.c_str(), &width, &height, &components, reqComp);
	}

	static unsigned char* loadTexture(const void* data, unsigned int size, int& width, int& height, int& components, int reqComp)
	{
		return stbi_load_from_memory((const stbi_uc*)data, size, &width, &height, &components, reqComp);
	}

	static void setTextureFlip(bool flip)
	{
		stbi_set_flip_vertically_on_load(flip);
	}

	static void unloadTexture(unsigned char* image)
	{
		stbi_image_free(image);
	}

private:
	void init(unsigned char* image, int width, int height, int components)
	{
		aux::TextureFormat format = aux::TextureFormat::red;
		if (components == 3)
		{
			format = aux::TextureFormat::rgb;
		}
		if (components == 4)
		{
			format = aux::TextureFormat::rgba;
		}

		generate();
		bind();

		aux::Texture texture{};
		texture.target = aux::toGLenum(aux::TextureTarget::two_d);
		texture.level = 0;
		texture.internalformat = aux::toGLenum(format);
		texture.width = width;
		texture.height = height;
		texture.border = 0;
		texture.format = aux::toGLenum(format);
		texture.type = aux::toGLenum(aux::Type::ubyte_type);
		texture.pixels = image;

		glTexImage2D(texture.target, texture.level, texture.format, texture.width, texture.height, texture.border, texture.format, texture.type, texture.pixels);
		glTexParameteri(texture.target, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(texture.target, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(texture.target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(texture.target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glGenerateMipmap(texture.target);

		unbind();
		unloadTexture(image);
	}

	void generate()
	{
		glGenTextures(1, &_tex);
	}

	void del()
	{
		glDeleteTextures(1, &_tex);
		_tex = 0;
	}

	unsigned int _tex{ 0 };

};
