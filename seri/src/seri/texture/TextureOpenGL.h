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

		void Init(const TextureDesc& desc) override
		{
			_desc = desc;
			_image = nullptr;

			Init();
			Build();
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

		void Init(const TextureDesc& desc, unsigned int width, unsigned int height) override
		{
			_desc = desc;
			_width = width;
			_height = height;
			_components = 0;
			_image = nullptr;

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

		int GetSlot() override
		{
			return static_cast<int>(_desc.slot);
		}

		uint32_t GetHandle() override
		{
			return _handle;
		}

		bool IsActiveForUsing() override
		{
			return _handle > 0;
		}

		void Bind() override
		{
			glActiveTexture(_slot);
			glBindTexture(_target, _handle);
		}

		void Bind(int slot) override
		{
			glActiveTexture(GL_TEXTURE0 + _slot);
			glBindTexture(_target, _handle);
		}

		void Unbind() override
		{
			glActiveTexture(_slot);
			glBindTexture(_target, 0);
		}

		void Unbind(int slot) override
		{
			glActiveTexture(GL_TEXTURE0 + _slot);
			glBindTexture(_target, 0);
		}

		void Clear(int val) override
		{
			Bind();
			glClearTexImage(_handle, 0, GL_RED_INTEGER, GL_INT, &val);
			Unbind();
		}

		void Clear(const glm::vec4& color) override
		{
			Bind();
			glClearTexImage(_handle, 0, GL_RGBA, GL_FLOAT, &color[0]);
			Unbind();
		}

		void Release() override
		{
			if (_handle > 0)
			{
				Unbind();
				Del();
			}
		}

		void LoadCubeMap(const std::vector<std::string>& faces) override
		{
			Bind();

			int width, height, components;
			for (size_t i = 0; i < faces.size(); i++)
			{
				int index = i;
				if (auto image = TextureBase::LoadTexture(faces[index], width, height, components, _desc.flip))
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
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + static_cast<GLenum>(index), 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image);
					TextureBase::UnloadTexture(image);
				}
				else
				{
					LOGGER(error, "[texture] texture " << faces[i] << " could not loaded for cube map");
				}
			}

			Unbind();
		}

	private:
		void Init()
		{
			if (_desc.format == TextureFormat::none)
			{
				if (_components == 3)
				{
					_desc.format = TextureFormat::rgb__rgb8ubyte;
					_internalformat = GetInternalFormatOpenGL(_desc.format);
				}
				else if (_components == 4)
				{
					_desc.format = TextureFormat::rgba__rgba8ubyte;
					_internalformat = GetInternalFormatOpenGL(_desc.format);
				}
			}

			_slot = GetSlotOpenGL(_desc.slot);
			_format = GetFormatOpenGL(_desc.format);
			_internalformat = GetInternalFormatOpenGL(_desc.format);
			_dataType = GetDataTypeFromFormatOpenGL(_desc.format);
			_target = GetTargetOpenGL(_desc.target);
			_wrapS = GetWrapOpenGL(_desc.wrapS);
			_wrapT = GetWrapOpenGL(_desc.wrapT);
			_wrapR = GetWrapOpenGL(_desc.wrapR);
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

		void Build()
		{
			Generate();
			Bind();

			if (_desc.target != TextureTarget::cube_map)
			{
				const GLint border = 0;
				glTexImage2D(_target, _desc.mip, _internalformat, _width, _height, border, _format, _dataType, _image);

				GLenum err = glGetError();
				if (err != GL_NO_ERROR)
				{
					switch (err)
					{
						case GL_INVALID_ENUM:
							LOGGER(error, "[texture] error: GL_INVALID_ENUM");
							break;
						case GL_INVALID_VALUE:
							LOGGER(error, "[texture] error: GL_INVALID_VALUE");
							break;
						case GL_INVALID_OPERATION:
							LOGGER(error, "[texture] error: GL_INVALID_OPERATION");
							break;
						case GL_OUT_OF_MEMORY:
							LOGGER(error, "[texture] error: GL_OUT_OF_MEMORY");
							break;
						default:
							LOGGER(error, "[texture] error: unknown: " << err);
							break;
					}
				}
			}

			glTexParameteri(_target, GL_TEXTURE_WRAP_S, _wrapS);
			glTexParameteri(_target, GL_TEXTURE_WRAP_T, _wrapT);
			glTexParameteri(_target, GL_TEXTURE_WRAP_R, _wrapR);

			glTexParameteri(_target, GL_TEXTURE_MAG_FILTER, _magFilter);
			glTexParameteri(_target, GL_TEXTURE_MIN_FILTER, _minFilter);

			glGenerateMipmap(_target);

			Unbind();
			UnloadTexture(_image);
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
				case seri::TextureDataType::half_float_type: return GL_HALF_FLOAT;
				case seri::TextureDataType::full_float_type: return GL_FLOAT;
				case seri::TextureDataType::double_type: return GL_DOUBLE;
			}

			return GL_BYTE;
		};

		GLenum GetFormatOpenGL(TextureFormat format)
		{
			switch (format)
			{
				case seri::TextureFormat::red__red8ubyte: return GL_RED;
				case seri::TextureFormat::red__red32int: return GL_RED_INTEGER;

				case seri::TextureFormat::rgb__rgb8ubyte: return GL_RGB;
				case seri::TextureFormat::rgb__rgb16float: return GL_RGB;
				case seri::TextureFormat::rgb__rgb32float: return GL_RGB;

				case seri::TextureFormat::rgba__rgba8ubyte: return GL_RGBA;

				case seri::TextureFormat::depth__depth24: return GL_DEPTH_COMPONENT;
				case seri::TextureFormat::depth_stencil__depth24_stencil8: return GL_DEPTH_STENCIL;
			}

			return 0;
		};

		GLenum GetInternalFormatOpenGL(TextureFormat format)
		{
			switch (format)
			{
				case seri::TextureFormat::red__red8ubyte: return GL_R8;
				case seri::TextureFormat::red__red32int: return GL_R32I;

				case seri::TextureFormat::rgb__rgb8ubyte: return GL_RGB8;
				case seri::TextureFormat::rgb__rgb16float: return GL_RGB16F;
				case seri::TextureFormat::rgb__rgb32float: return GL_RGB32F;

				case seri::TextureFormat::rgba__rgba8ubyte: return GL_RGBA8;

				case seri::TextureFormat::depth__depth24: return GL_DEPTH_COMPONENT24;
				case seri::TextureFormat::depth_stencil__depth24_stencil8: return GL_DEPTH24_STENCIL8;
			}

			return 0;
		}

		GLenum GetDataTypeFromFormatOpenGL(TextureFormat format)
		{
			switch (format)
			{
				case seri::TextureFormat::red__red8ubyte: return GL_UNSIGNED_BYTE;
				case seri::TextureFormat::red__red32int: return GL_INT;

				case seri::TextureFormat::rgb__rgb8ubyte: return GL_UNSIGNED_BYTE;
				case seri::TextureFormat::rgb__rgb16float: return GL_HALF_FLOAT;
				case seri::TextureFormat::rgb__rgb32float: return GL_FLOAT;

				case seri::TextureFormat::rgba__rgba8ubyte: return GL_UNSIGNED_BYTE;

				case seri::TextureFormat::depth__depth24: return GL_UNSIGNED_INT;
				case seri::TextureFormat::depth_stencil__depth24_stencil8: return GL_UNSIGNED_INT_24_8;
			}

			return 0;
		}

		GLenum _slot;
		GLenum _format;
		GLenum _internalformat;
		GLenum _target;
		GLenum _wrapS;
		GLenum _wrapT;
		GLenum _wrapR;
		GLenum _dataType;
		GLenum _magFilter;
		GLenum _minFilter;

		TextureDesc _desc;
		unsigned int _handle{ 0 };

	};
}
