#pragma once

#include "seri/util/Util.h"

namespace seri
{
	enum class TextureSlot
	{
		uv0,
		uv1,
		uv2,
		uv3,
	};

	enum class TextureFormat
	{
		red,
		rgb,
		rgba,
		srgb,
		srgba,
	};

	enum class TextureTarget
	{
		two_d,
		three_d,
		cube_map,
		cube_map_negative_x,
		cube_map_negative_y,
		cube_map_negative_z,
		cube_map_positive_x,
		cube_map_positive_y,
		cube_map_positive_z,
	};

	enum class TextureWrap
	{
		none,
		repeat,
		clamp_to_edge,
		clamp_to_border,
		mirrored_repeat,
		mirror_clamp_to_edge,
	};

	enum class TextureMinFilter
	{
		linear,
		nearest,
		linear_mipmap_linear,
		nearest_mipmap_linear,
		linear_mipmap_nearest,
		nearest_mipmap_nearest,
	};

	enum class TextureMagFilter
	{
		linear,
		nearest,
	};

	enum class TextureDataType
	{
		byte_type,
		ubyte_type,
		short_type,
		ushort_type,
		int_type,
		uint_type,
		float_type,
		double_type,
	};

	struct TextureDesc
	{
		int mip{ 0 };
		int border{ 0 };
		bool flip{ true };
		TextureSlot slot{ TextureSlot::uv0 };
		TextureFormat format{ TextureFormat::rgba };
		TextureTarget target{ TextureTarget::two_d };
		TextureWrap wrap_s{ TextureWrap::repeat };
		TextureWrap wrap_t{ TextureWrap::repeat };
		TextureWrap wrap_r{ TextureWrap::none };
		TextureMagFilter magFilter{ TextureMagFilter::linear };
		TextureMinFilter minFilter{ TextureMinFilter::linear_mipmap_linear };
		TextureDataType dataType{ TextureDataType::ubyte_type };
	};

	class TextureBase
	{
	public:
		virtual ~TextureBase() = default;

		virtual void Init(const TextureDesc& desc) = 0;
		virtual void Init(const TextureDesc& desc, const std::string& texturePath) = 0;
		virtual void Init(const TextureDesc& desc, const void* data, unsigned int size) = 0;
		virtual void Init(const TextureDesc& desc, const void* data, unsigned int width, unsigned int height, unsigned int components) = 0;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;
		virtual void Release() = 0;

		virtual void LoadCubeMap(const std::vector<std::string>& faces) = 0;

		int GetWidth() { return _width; }
		int GetHeight() { return _height; }
		int GetComponents() { return _components; }
		void* GetImage() { return _image; }

		static void* LoadTexture(const std::string& texturePath, int& width, int& height, int& components, bool flip);
		static void* LoadTexture(const void* data, unsigned int size, int& width, int& height, int& components, bool flip);
		static void UnloadTexture(void* image);
		static void WriteTexture(const std::string& path, const void* data, int width, int height, int components, bool flip);

		static std::shared_ptr<TextureBase> Create();

	protected:
		int _width{ 0 };
		int _height{ 0 };
		int _reqComp{ 0 };
		int _components{ 0 };
		void* _image{ nullptr };

	};
}
