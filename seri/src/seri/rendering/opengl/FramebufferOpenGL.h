#pragma once

#include "seri/util/Util.h"
#include "seri/rendering/common/FramebufferBase.h"

#include <glad/gl.h>

namespace seri
{
	class FramebufferOpenGL : public FramebufferBase
	{
	public:
		FramebufferOpenGL();
		FramebufferOpenGL(FramebufferDesc desc);
		~FramebufferOpenGL() override;

		void Bind() override;
		void Unbind() override;

		uint32_t GetFirstColorTextureHandle() override;
		std::shared_ptr<TextureBase> GetDepthTexture() override;

		void Invalidate() override;

		void Resize(uint32_t width, uint32_t height) override;

	private:
		uint32_t _handle;

		float _ascpectRatio{ 0.0f };

		FramebufferTextureDesc _depthAttachment;
		std::vector<FramebufferTextureDesc> _colorAttachments;

		std::shared_ptr<TextureBase> _depthTexture{ nullptr };
		std::vector<std::shared_ptr<TextureBase>> _colorTextures{};

	};
}
