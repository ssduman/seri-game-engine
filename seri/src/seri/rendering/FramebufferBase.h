#pragma once

#include "seri/util/Util.h"
#include "seri/rendering/BufferBase.h"
#include "seri/rendering/RenderingUtil.h"

namespace seri
{
	enum class FramebufferTextureFormat
	{
		none,

		// color
		rgba8,
		red32,

		// depth and stencil
		depth24_stencil8,
	};

	struct FramebufferTextureDesc
	{
		FramebufferTextureDesc() = default;
		FramebufferTextureDesc(FramebufferTextureFormat format) : textureFormat(format) {}

		FramebufferTextureFormat textureFormat{ FramebufferTextureFormat::none };
	};

	struct FramebufferDesc
	{
		FramebufferDesc() = default;

		FramebufferDesc& AddColorAttachment(FramebufferTextureFormat textureFormat)
		{
			attachmentColors.emplace_back(textureFormat);
			return *this;
		}

		FramebufferDesc& SetDepthAndStencilAttachment(FramebufferTextureFormat textureFormat)
		{
			attachmentDepthAndStencil.textureFormat = textureFormat;
			return *this;
		}

		uint32_t width{ 1280 };
		uint32_t height{ 720 };
		FramebufferTextureDesc attachmentDepthAndStencil{};
		std::vector<FramebufferTextureDesc> attachmentColors{};
	};

	class FramebufferBase
	{
	public:
		virtual ~FramebufferBase() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		static std::shared_ptr<FramebufferBase> Create(FramebufferDesc desc);

	protected:
		FramebufferDesc _desc{};

	};

}
