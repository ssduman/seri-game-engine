#pragma once

#include "seri/util/Util.h"
#include "seri/rendering/FramebufferBase.h"

#include <glad/gl.h>

namespace seri
{
	class FramebufferOpenGL : public FramebufferBase
	{
	public:
		FramebufferOpenGL()
		{
			_handle = 0;
		}

		FramebufferOpenGL(FramebufferDesc desc)
		{
			for (auto& attachment : desc.attachments)
			{
				if (attachment.textureDesc.format == TextureFormat::none)
				{
					continue;
				}

				if (attachment.textureDesc.format == TextureFormat::depth__depth24 || attachment.textureDesc.format == TextureFormat::depth_stencil__depth24_stencil8)
				{
					_depthAttachment = attachment;
				}
				else
				{
					_colorAttachments.push_back(attachment);
				}
			}

			Invalidate();
		}

		~FramebufferOpenGL() override
		{
			if (_handle > 0)
			{
				glDeleteFramebuffers(1, &_handle);
			}
		}

		void Bind() override
		{
			glBindFramebuffer(GL_FRAMEBUFFER, _handle);
		}
		void Unbind() override
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		uint32_t GetFirstColorTextureHandle() override
		{
			if (_colorTextures.size() > 0 && _colorTextures[0] != nullptr)
			{
				return _colorTextures[0]->GetHandle();
			}

			return 0;
		}

		void Invalidate()
		{
			if (_handle > 0)
			{
				glDeleteFramebuffers(1, &_handle);

				_depthTexture = nullptr;

				for (auto& tex : _colorTextures)
				{
					tex = nullptr;
				}
				_colorTextures.clear();
			}

			glGenFramebuffers(1, &_handle);
			Bind();

			if (_depthAttachment.textureDesc.format != TextureFormat::none)
			{
				_depthTexture = TextureBase::Create();
				_depthTexture->Init(_depthAttachment.textureDesc, _desc.width, _desc.height);
			}

			for (size_t i = 0; i < _colorAttachments.size(); i++)
			{
				auto& attachment = _colorAttachments[i];

				std::shared_ptr<TextureBase> _tex = TextureBase::Create();
				_tex->Init(attachment.textureDesc, _desc.width, _desc.height);
				_colorTextures.push_back(_tex);

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, _tex->GetHandle(), attachment.textureDesc.mip);
			}

			if (_colorTextures.size() > 0)
			{
				GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
				glDrawBuffers(_colorTextures.size(), buffers);
			}
			else
			{
				glDrawBuffer(GL_NONE);
			}

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
			{
				LOGGER(error, "framebuffer is not completed");
			}

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

	private:
		uint32_t _handle;

		FramebufferTextureDesc _depthAttachment;
		std::vector<FramebufferTextureDesc> _colorAttachments;

		std::shared_ptr<TextureBase> _depthTexture{ nullptr };
		std::vector<std::shared_ptr<TextureBase>> _colorTextures{};

	};
}
