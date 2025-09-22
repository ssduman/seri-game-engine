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
			glGenFramebuffers(1, &_handle);
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

	private:
		unsigned int _handle;

	};
}
