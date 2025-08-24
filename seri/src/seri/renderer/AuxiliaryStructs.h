#pragma once

#include <glad/gl.h>

#include <vector>

// enum class
namespace seri::aux
{
	enum class Dimension : int
	{
		two_d = 2,
		three_d = 3,
	};

	enum class Index : GLuint
	{
		position = 0,
		texture = 1,
		color = 2,
		normal = 3,
		skin_bone_id = 4,
		skin_weight = 5,
		instanced_mat4 = 6,
		instanced_mat4_reserved_1 = 7,
		instanced_mat4_reserved_2 = 8,
		instanced_mat4_reserved_3 = 9,
	};

	enum class Type : GLenum
	{
		byte_type = GL_BYTE,
		ubyte_type = GL_UNSIGNED_BYTE,
		short_type = GL_SHORT,
		ushort_type = GL_UNSIGNED_SHORT,
		int_type = GL_INT,
		uint_type = GL_UNSIGNED_INT,
		float_type = GL_FLOAT,
		double_type = GL_DOUBLE,
	};

	enum class DrawMode : GLenum
	{
		points = GL_POINTS,
		lines = GL_LINES,
		line_loop = GL_LINE_LOOP,
		line_strip = GL_LINE_STRIP,
		triangles = GL_TRIANGLES,
		triangle_strip = GL_TRIANGLE_STRIP,
		triangle_fan = GL_TRIANGLE_FAN,
	};

	enum class DrawType : int
	{
		arrays = 0,
		elements = 1,
	};

	enum class Target : GLenum
	{
		vbo = GL_ARRAY_BUFFER,
		ebo = GL_ELEMENT_ARRAY_BUFFER,
	};

	enum class Usage : GLenum
	{
		stream_draw = GL_STREAM_DRAW,
		static_draw = GL_STATIC_DRAW,
		dynamic_draw = GL_DYNAMIC_DRAW,
	};

	enum class Access : GLenum
	{
		read = GL_READ_ONLY,
		write = GL_WRITE_ONLY,
		read_write = GL_READ_WRITE,
	};

	enum class Component : GLenum
	{
		one = 1,
		two = 2,
		three = 3,
		four = 4,
		bgra = GL_BGRA,
	};

	enum class TextureTarget : GLenum
	{
		two_d = GL_TEXTURE_2D,
		three_d = GL_TEXTURE_3D,
		cube_map = GL_TEXTURE_CUBE_MAP,
		cube_map_negative_x = GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		cube_map_negative_y = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		cube_map_negative_z = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
		cube_map_positive_x = GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		cube_map_positive_y = GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		cube_map_positive_z = GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
	};

	enum class TextureFormat : GLenum
	{
		red = GL_RED,
		rgb = GL_RGB,
		rgba = GL_RGBA,
		srgb = GL_SRGB,
		srgba = GL_SRGB_ALPHA,
	};

	enum class TextureParamName : GLenum
	{
		depth_stencil_mode = GL_DEPTH_STENCIL_TEXTURE_MODE,
		base_level = GL_TEXTURE_BASE_LEVEL,
		compare_func = GL_TEXTURE_COMPARE_FUNC,
		compare_mode = GL_TEXTURE_COMPARE_MODE,
		lod_bias = GL_TEXTURE_LOD_BIAS,
		min_filter = GL_TEXTURE_MIN_FILTER,
		mag_filter = GL_TEXTURE_MAG_FILTER,
		min_lod = GL_TEXTURE_MIN_LOD,
		max_lod = GL_TEXTURE_MAX_LOD,
		max_level = GL_TEXTURE_MAX_LEVEL,
		swizzle_r = GL_TEXTURE_SWIZZLE_R,
		swizzle_g = GL_TEXTURE_SWIZZLE_G,
		swizzle_b = GL_TEXTURE_SWIZZLE_B,
		swizzle_a = GL_TEXTURE_SWIZZLE_A,
		wrap_s = GL_TEXTURE_WRAP_S,
		wrap_t = GL_TEXTURE_WRAP_T,
		wrap_r = GL_TEXTURE_WRAP_R,
	};

	enum class TextureWrap : GLenum
	{
		clamp_to_edge = GL_CLAMP_TO_EDGE,
		clamp_to_border = GL_CLAMP_TO_BORDER,
		repeat = GL_REPEAT,
		mirrored_repeat = GL_MIRRORED_REPEAT,
		mirror_clamp_to_edge = GL_MIRROR_CLAMP_TO_EDGE,
	};

	enum class TextureMinFilter : GLenum
	{
		linear = GL_LINEAR,
		nearest = GL_NEAREST,
		linear_mipmap_linear = GL_LINEAR_MIPMAP_LINEAR,
		nearest_mipmap_linear = GL_NEAREST_MIPMAP_LINEAR,
		linear_mipmap_nearest = GL_LINEAR_MIPMAP_NEAREST,
		nearest_mipmap_nearest = GL_NEAREST_MIPMAP_NEAREST,
	};

	enum class TextureMagFilter : GLenum
	{
		linear = GL_LINEAR,
		nearest = GL_NEAREST,
	};
}

// function
namespace seri::aux
{
	template<typename T>
	int length(const typename std::vector<T>& vec)
	{
		return static_cast<int>(T::length());
	}

	template<typename T>
	int component(const typename std::vector<T>& vec)
	{
		return static_cast<int>(T::length());
	}

	template<typename T>
	GLsizei count(const typename std::vector<T>& vec)
	{
		return static_cast<GLsizei>(vec.size());
	}

	template<typename T>
	GLsizei stride(const typename std::vector<T>& vec)
	{
		return static_cast<GLsizei>(sizeof(T) * aux::length(vec));
	}

	template<typename T>
	GLsizeiptr size(const typename std::vector<T>& vec)
	{
		return static_cast<GLsizeiptr>(sizeof(T) * vec.size());
	}

	template<typename T>
	GLsizeiptr capacity(const typename std::vector<T>& vec)
	{
		return static_cast<GLsizeiptr>(sizeof(T) * vec.capacity());
	}

	template<typename T>
	const void* data(const typename std::vector<T>& vec)
	{
		return vec.data();
	}

	template <typename Enumeration>
	auto toInt(Enumeration const value) -> typename std::underlying_type<Enumeration>::type
	{
		return static_cast<typename std::underlying_type<Enumeration>::type>(value);
	}

	template <typename Enumeration>
	auto toUInt(Enumeration const value) -> typename std::underlying_type<Enumeration>::type
	{
		return static_cast<typename std::underlying_type<Enumeration>::type>(value);
	}

	template <typename Enumeration>
	auto toGLenum(Enumeration const value) -> typename std::underlying_type<Enumeration>::type
	{
		return static_cast<typename std::underlying_type<Enumeration>::type>(value);
	}
}

// struct
namespace seri::aux
{
	struct Attribute
	{
		Attribute() = default;

		Attribute(Index index_, GLint size_, const void* pointer_)
			:
			index(toUInt(index_)),
			size(size_),
			stride(size * sizeof(GLfloat)),
			pointer(pointer_)
		{
		}

		GLuint index = 0;
		GLint size = 0;
		GLenum type = GL_FLOAT;
		GLboolean normalized = GL_FALSE;
		GLsizei stride = 0;
		const void* pointer = nullptr;
	};

	struct DataBuffer
	{
		DataBuffer() = default;

		DataBuffer(GLsizeiptr size_)
			:
			size(size_)
		{
		}

		DataBuffer(GLsizeiptr size_, const void* data_)
			:
			size(size_),
			data(data_)
		{
		}

		DataBuffer(Target target_, GLsizeiptr size_, const void* data_)
			:
			target(toGLenum(target_)),
			size(size_),
			data(data_)
		{
		}

		DataBuffer(Target target_, GLsizeiptr size_, const void* data_, Usage usage_)
			:
			target(toGLenum(target_)),
			size(size_),
			data(data_),
			usage(toGLenum(usage_))
		{
		}

		GLenum target = GL_ARRAY_BUFFER;
		GLsizeiptr size = 0;
		const void* data = nullptr;
		GLenum usage = GL_STATIC_DRAW;
	};

	struct SubDataBuffer
	{
		SubDataBuffer() = default;

		SubDataBuffer(GLintptr offset_, GLsizeiptr size_, const void* data_)
			:
			offset(offset_),
			size(size_),
			data(data_)
		{
		}

		SubDataBuffer(Target target_, GLintptr offset_, GLsizeiptr size_, const void* data_)
			:
			target(toGLenum(target_)),
			offset(offset_),
			size(size_),
			data(data_)
		{
		}

		GLenum target = GL_ARRAY_BUFFER;
		GLintptr offset = 0;
		GLsizeiptr size = 0;
		const void* data = nullptr;
	};

	struct MapBuffer
	{
		MapBuffer() = default;

		MapBuffer(size_t size_, const void* data_)
			:
			size(size_),
			data(data_)
		{
		}

		MapBuffer(Target target_, Access access_, size_t size_, const void* data_)
			:
			target(toGLenum(target_)),
			access(toGLenum(access_)),
			size(size_),
			data(data_)
		{
		}

		GLenum target = GL_ARRAY_BUFFER;
		GLenum access = GL_WRITE_ONLY;
		size_t size = 0;
		const void* data = nullptr;
	};

	struct Texture
	{
		GLenum target;
		GLint level;
		GLint internalformat;
		GLsizei width;
		GLsizei height;
		GLint border;
		GLenum format;
		GLenum type;
		const void* pixels;
	};

	struct DrawElement
	{
		GLenum mode;
		GLsizei count;
		GLenum type;
		const void* indices;
	};

	struct DrawArray
	{
		GLenum mode;
		GLint first;
		GLsizei count;
	};

}
