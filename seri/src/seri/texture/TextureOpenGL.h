#pragma once

#include "seri/texture/TextureBase.h"

#include <glad/gl.h>

namespace seri
{
	class TextureOpenGL : public TextureBase
	{
	public:
		~TextureOpenGL() override
		{
			Release();
		}

		void Init(const TextureDesc& desc, const std::string& texturePath) override
		{
			_desc = desc;
			_image = TextureBase::LoadTexture(texturePath, _width, _height, _components, desc.flip);

			Init();
			Build();
		}

		void Init(const TextureDesc& desc, const void* data, unsigned int size) override
		{
			_desc = desc;
			_image = TextureBase::LoadTexture(data, size, _width, _height, _components, desc.flip);

			Init();
			Build();
		}

		void Init(const TextureDesc& desc, const void* data, unsigned int width, unsigned int height, unsigned int components) override
		{
			_desc = desc;
			_width = width;
			_height = height;
			_components = components;
			_image = (void*)data;

			Init();
			Build();
		}

		void Build()
		{
			if (!_image)
			{
				LOGGER(error, "[texture] no image data");
				return;
			}

			Generate();
			Bind();

			glTexImage2D(_target, _desc.mip, _format, _width, _height, _desc.border, _format, _dataType, _image);
			glTexParameteri(_target, GL_TEXTURE_WRAP_S, _wrap);
			glTexParameteri(_target, GL_TEXTURE_WRAP_T, _wrap);
			glTexParameteri(_target, GL_TEXTURE_MAG_FILTER, _magFilter);
			glTexParameteri(_target, GL_TEXTURE_MIN_FILTER, _minFilter);
			glGenerateMipmap(_target);

			Unbind();
			UnloadTexture(_image);
		}

		void Bind() override
		{
			glActiveTexture(_slot);
			glBindTexture(_target, _handle);
		}

		void Unbind() override
		{
			glBindTexture(_target, 0);
		}

		void Release() override
		{
			if (_handle > 0)
			{
				Unbind();
				Del();
			}
		}

	private:
		void Init()
		{
			_desc.format = TextureFormat::red;
			if (_components == 3)
			{
				_desc.format = TextureFormat::rgb;
			}
			else if (_components == 4)
			{
				_desc.format = TextureFormat::rgba;
			}

			_slot = GetSlotOpenGL(_desc.slot);
			_format = GetFormatOpenGL(_desc.format);
			_target = GetTargetOpenGL(_desc.target);
			_wrap = GetWrapOpenGL(_desc.wrap);
			_dataType = GetDataTypeOpenGL(_desc.dataType);
			_magFilter = GetMagFilterOpenGL(_desc.magFilter);
			_minFilter = GetMinFilterOpenGL(_desc.minFilter);
		}

		void Generate()
		{
			glGenTextures(1, &_handle);
		}

		void Del()
		{
			if (_handle > 0)
			{
				glDeleteTextures(1, &_handle);
				_handle = 0;
			}
		}

		GLenum GetSlotOpenGL(TextureSlot slot)
		{
			switch (slot)
			{
				case seri::TextureSlot::uv0: return GL_TEXTURE0;
				case seri::TextureSlot::uv1: return GL_TEXTURE1;
				case seri::TextureSlot::uv2: return GL_TEXTURE2;
				case seri::TextureSlot::uv3: return GL_TEXTURE3;
			}

			return GL_TEXTURE0;
		};

		GLenum GetFormatOpenGL(TextureFormat format)
		{
			switch (format)
			{
				case seri::TextureFormat::red: return GL_RED;
				case seri::TextureFormat::rgb: return GL_RGB;
				case seri::TextureFormat::rgba: return GL_RGBA;
				case seri::TextureFormat::srgb: return GL_SRGB;
				case seri::TextureFormat::srgba: return GL_SRGB_ALPHA;
			}

			return GL_RED;
		};

		GLenum GetTargetOpenGL(TextureTarget target)
		{
			switch (target)
			{
				case seri::TextureTarget::two_d: return GL_TEXTURE_2D;
				case seri::TextureTarget::three_d: return GL_TEXTURE_3D;
				case seri::TextureTarget::cube_map: return GL_TEXTURE_CUBE_MAP;
				case seri::TextureTarget::cube_map_negative_x: return GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
				case seri::TextureTarget::cube_map_negative_y: return GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
				case seri::TextureTarget::cube_map_negative_z: return GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
				case seri::TextureTarget::cube_map_positive_x: return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
				case seri::TextureTarget::cube_map_positive_y: return GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
				case seri::TextureTarget::cube_map_positive_z: return GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
			}

			return GL_TEXTURE_2D;
		};

		GLenum GetWrapOpenGL(TextureWrap wrap)
		{
			switch (wrap)
			{
				case seri::TextureWrap::repeat: return GL_REPEAT;
				case seri::TextureWrap::clamp_to_edge: return GL_CLAMP_TO_EDGE;
				case seri::TextureWrap::clamp_to_border: return GL_CLAMP_TO_BORDER;
				case seri::TextureWrap::mirrored_repeat: return GL_MIRRORED_REPEAT;
				case seri::TextureWrap::mirror_clamp_to_edge: return GL_MIRROR_CLAMP_TO_EDGE;
			}

			return GL_REPEAT;
		};

		GLenum GetMagFilterOpenGL(TextureMagFilter magFilter)
		{
			switch (magFilter)
			{
				case seri::TextureMagFilter::linear: return GL_LINEAR;
				case seri::TextureMagFilter::nearest: return GL_NEAREST;
			}

			return GL_LINEAR;
		};

		GLenum GetMinFilterOpenGL(TextureMinFilter minFilter)
		{
			switch (minFilter)
			{
				case seri::TextureMinFilter::linear: return GL_LINEAR;
				case seri::TextureMinFilter::nearest: return GL_NEAREST;
				case seri::TextureMinFilter::linear_mipmap_linear: return GL_LINEAR_MIPMAP_LINEAR;
				case seri::TextureMinFilter::nearest_mipmap_linear: return GL_NEAREST_MIPMAP_LINEAR;
				case seri::TextureMinFilter::linear_mipmap_nearest: return GL_LINEAR_MIPMAP_NEAREST;
				case seri::TextureMinFilter::nearest_mipmap_nearest: return GL_NEAREST_MIPMAP_NEAREST;
			}

			return GL_LINEAR;
		};

		GLenum GetDataTypeOpenGL(TextureDataType dataType)
		{
			switch (dataType)
			{
				case seri::TextureDataType::byte_type: return GL_BYTE;
				case seri::TextureDataType::ubyte_type: return GL_UNSIGNED_BYTE;
				case seri::TextureDataType::short_type: return GL_SHORT;
				case seri::TextureDataType::ushort_type: return GL_UNSIGNED_SHORT;
				case seri::TextureDataType::int_type: return GL_INT;
				case seri::TextureDataType::uint_type: return GL_UNSIGNED_INT;
				case seri::TextureDataType::float_type: return GL_FLOAT;
				case seri::TextureDataType::double_type: return GL_DOUBLE;
			}

			return GL_BYTE;
		};

		GLenum _slot;
		GLenum _format;
		GLenum _target;
		GLenum _wrap;
		GLenum _dataType;
		GLenum _magFilter;
		GLenum _minFilter;
		TextureDesc _desc;
		unsigned int _handle{ 0 };

	};
}
