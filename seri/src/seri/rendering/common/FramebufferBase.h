#pragma once

#include "seri/util/Util.h"
#include "seri/texture/TextureBase.h"
#include "seri/rendering/common/BufferBase.h"
#include "seri/rendering/render/RenderingUtil.h"

namespace seri
{
	struct FramebufferTextureDesc
	{
		FramebufferTextureDesc()
		{
			textureDesc.format = TextureFormat::none;
		}

		FramebufferTextureDesc(TextureDesc desc) : textureDesc(desc) {}

		TextureDesc textureDesc{};
	};

	struct FramebufferDesc
	{
		FramebufferDesc() = default;

		void AddAttachments(std::initializer_list<FramebufferTextureDesc> attachments)
		{
			for (auto attachment : attachments)
			{
				this->attachments.emplace_back(attachment);
			}
		}

		uint32_t width{ 1280 };
		uint32_t height{ 720 };
		bool fixedAspectRatio{ true };
		std::vector<FramebufferTextureDesc> attachments{};
	};

	class FramebufferBase
	{
	public:
		virtual ~FramebufferBase() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void Invalidate() = 0;
		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual uint32_t GetFirstColorTextureHandle() = 0;

		uint32_t GetWidth()
		{
			return _desc.width;
		}
		uint32_t GetHeight()
		{
			return _desc.height;
		}
		float GetAspectRatio()
		{
			return static_cast<float>(_desc.width) / static_cast<float>(_desc.height);
		}

		static std::shared_ptr<FramebufferBase> Create();
		static std::shared_ptr<FramebufferBase> Create(FramebufferDesc desc);

	protected:
		FramebufferDesc _desc{};

	};

}
