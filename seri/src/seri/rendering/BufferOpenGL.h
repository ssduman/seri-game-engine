#pragma once

#include "seri/util/Util.h"
#include "seri/rendering/BufferBase.h"

#include <glad/gl.h>

namespace seri
{
	class BufferOpenGL : public BufferBase
	{
	public:
		BufferOpenGL(const BufferDesc& desc, const void* data) : _desc(desc), _handle(0)
		{
			_usage = GetUsageOpenGL(desc.usage);
			_target = GetTargetOpenGL(desc.target);

			glGenBuffers(1, &_handle);

			glBindBuffer(_target, _handle);
			glBufferData(_target, desc.size, data, _usage);
			glBindBuffer(_target, 0);
		}

		~BufferOpenGL() override
		{
			if (_handle > 0)
			{
				glDeleteBuffers(1, &_handle);
			}
		}

		void UpdateBuffer(const void* src, size_t size, size_t offset = 0)
		{
			glBindBuffer(_target, _handle);

			if (_desc.usage == BufferUsage::dynamic_draw)
			{
				void* ptr = glMapBufferRange(_target, offset, size, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);
				if (ptr)
				{
					memcpy(ptr, src, size);
					glUnmapBuffer(_target);
				}
			}
			else
			{
				glBufferSubData(_target, offset, size, src);
			}

			glBindBuffer(_target, 0);
		}

	private:
		GLenum GetTargetOpenGL(BufferTarget target)
		{
			switch (target)
			{
				case BufferTarget::vertex: return GL_ARRAY_BUFFER;
				case BufferTarget::index: return GL_ELEMENT_ARRAY_BUFFER;
			}

			return GL_ARRAY_BUFFER;
		}

		GLenum GetUsageOpenGL(BufferUsage usage)
		{
			switch (usage)
			{
				case BufferUsage::stream_draw: return GL_STREAM_DRAW;
				case BufferUsage::static_draw: return GL_STATIC_DRAW;
				case BufferUsage::dynamic_draw: return GL_DYNAMIC_DRAW;
			}

			return GL_STATIC_DRAW;
		};

		GLenum _usage;
		GLenum _target;
		BufferDesc _desc;
		unsigned int _handle;

	};
}
