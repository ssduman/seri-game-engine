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
		none,

		red__red8ubyte,
		red__red32int,

		rgb__rgb8ubyte,
		rgb__rgb16float,
		rgb__rgb32float,

		rgba__rgba8ubyte,

		depth__depth24,
		depth_stencil__depth24_stencil8,
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
		half_float_type,
		full_float_type,
		double_type,
	};

	struct TextureDesc
	{
		int mip{ 0 };
		bool flip{ true };
		TextureSlot slot{ TextureSlot::uv0 };
		TextureFormat format{ TextureFormat::none };
		TextureTarget target{ TextureTarget::two_d };
		TextureWrap wrapS{ TextureWrap::clamp_to_edge };
		TextureWrap wrapT{ TextureWrap::clamp_to_edge };
		TextureWrap wrapR{ TextureWrap::clamp_to_edge };
		TextureMagFilter magFilter{ TextureMagFilter::linear };
		TextureMinFilter minFilter{ TextureMinFilter::linear };
	};

	class TextureBase
	{
	public:
		virtual ~TextureBase() = default;

		virtual void Init(const TextureDesc& desc) = 0;
		virtual void Init(const TextureDesc& desc, const std::string& texturePath) = 0;
		virtual void Init(const TextureDesc& desc, const void* data, unsigned int size) = 0;
		virtual void Init(const TextureDesc& desc, unsigned int width, unsigned int height) = 0;
		virtual void Init(const TextureDesc& desc, const void* data, unsigned int width, unsigned int height, unsigned int components) = 0;

		virtual int GetSlot() = 0;
		virtual uint32_t GetHandle() = 0;
		virtual bool IsActiveForUsing() = 0;

		virtual void Bind() = 0;
		virtual void Bind(int slot) = 0;
		virtual void Unbind() = 0;
		virtual void Unbind(int slot) = 0;
		virtual void Clear(int val) = 0;
		virtual void Clear(const glm::vec4& color) = 0;
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
