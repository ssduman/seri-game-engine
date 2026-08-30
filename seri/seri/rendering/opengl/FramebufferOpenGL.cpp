#include "Seripch.h"

#include "seri/rendering/opengl/FramebufferOpenGL.h"

namespace seri
{
	FramebufferOpenGL::FramebufferOpenGL()
	{
		_handle = 0;
	}

	FramebufferOpenGL::FramebufferOpenGL(FramebufferDesc desc)
	{
		_desc = desc;
		_ascpectRatio = GetAspectRatio();

		for (auto& attachment : desc.attachments)
		{
			if (attachment.textureDesc.format == TextureFormat::none)
			{
				continue;
			}

			if (attachment.textureDesc.format == TextureFormat::depth__depth24 || attachment.textureDesc.format == TextureFormat::depth__depth32float || attachment.textureDesc.format == TextureFormat::depth_stencil__depth24_stencil8)
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

	FramebufferOpenGL::~FramebufferOpenGL()
	{
		if (_handle > 0)
		{
			glDeleteFramebuffers(1, &_handle);
		}
	}

	void FramebufferOpenGL::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, _handle);
	}

	void FramebufferOpenGL::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	uint32_t FramebufferOpenGL::GetFirstColorTextureHandle()
	{
		if (_colorTextures.size() > 0 && _colorTextures[0] != nullptr)
		{
			return _colorTextures[0]->GetHandle();
		}

		return 0;
	}

	std::shared_ptr<TextureBase> FramebufferOpenGL::GetDepthTexture()
	{
		return _depthTexture;
	}

	void FramebufferOpenGL::Invalidate()
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

			GLenum attachmentType = (_depthAttachment.textureDesc.format == TextureFormat::depth__depth24 || _depthAttachment.textureDesc.format == TextureFormat::depth__depth32float) ? GL_DEPTH_ATTACHMENT : GL_DEPTH_STENCIL_ATTACHMENT;
			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, GL_TEXTURE_2D, _depthTexture->GetHandle(), _depthAttachment.textureDesc.mip);
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

		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			switch (status)
			{
				case GL_FRAMEBUFFER_UNDEFINED:
					LOGGER(error, "[framebuffer] error: GL_FRAMEBUFFER_UNDEFINED");
					break;
				case GL_FRAMEBUFFER_UNSUPPORTED:
					LOGGER(error, "[framebuffer] error: GL_FRAMEBUFFER_UNSUPPORTED");
					break;
				case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
					LOGGER(error, "[framebuffer] error: GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT");
					break;
				case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
					LOGGER(error, "[framebuffer] error: GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT");
					break;
				case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
					LOGGER(error, "[framebuffer] error: GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER");
					break;
				case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
					LOGGER(error, "[framebuffer] error: GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER");
					break;
				case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
					LOGGER(error, "[framebuffer] error: GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE");
					break;
				case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
					LOGGER(error, "[framebuffer] error: GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS");
					break;
				default:
					LOGGER(error, "[framebuffer] error: unknown: " << status);
			}
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FramebufferOpenGL::Resize(uint32_t width, uint32_t height)
	{
		if (_desc.fixedAspectRatio)
		{
			//height = static_cast<uint32_t>(width * (1.0f / _ascpectRatio));
		}

		_desc.width = width;
		_desc.height = height;

		Invalidate();
	}

}
