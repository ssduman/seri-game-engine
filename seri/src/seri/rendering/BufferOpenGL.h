#pragma once

#include "seri/util/Util.h"
#include "seri/rendering/BufferBase.h"

#include <glad/gl.h>

namespace seri
{
	GLenum GetBufferTargetOpenGL(BufferTarget target)
	{
		switch (target)
		{
			case BufferTarget::vertex: return GL_ARRAY_BUFFER;
			case BufferTarget::index: return GL_ELEMENT_ARRAY_BUFFER;
		}

		return GL_ARRAY_BUFFER;
	}

	GLenum GetBufferUsageOpenGL(BufferUsage usage)
	{
		switch (usage)
		{
			case BufferUsage::stream_draw: return GL_STREAM_DRAW;
			case BufferUsage::static_draw: return GL_STATIC_DRAW;
			case BufferUsage::dynamic_draw: return GL_DYNAMIC_DRAW;
		}

		return GL_STATIC_DRAW;
	};

	class IndexBufferOpenGL : public IndexBufferBase
	{
	public:
		IndexBufferOpenGL(const uint32_t* data, uint32_t count)
		{
			_count = count;

			_desc.usage = BufferUsage::static_draw;
			_desc.target = BufferTarget::index;

			_usage = GetBufferUsageOpenGL(_desc.usage);
			_target = GetBufferTargetOpenGL(_desc.target);

			uint32_t size = count * sizeof(uint32_t);

			glGenBuffers(1, &_handle);

			glBindBuffer(GetBufferTargetOpenGL(BufferTarget::vertex), _handle);
			glBufferData(GetBufferTargetOpenGL(BufferTarget::vertex), size, data, _usage);

			Unbind();
		}

		~IndexBufferOpenGL() override
		{
			if (_handle > 0)
			{
				glDeleteBuffers(1, &_handle);
			}
		}

		void Bind() override
		{
			glBindBuffer(_target, _handle);
		}
		void Unbind() override
		{
			glBindBuffer(_target, 0);
		}

		void SetData(const void* data, uint32_t count, uint32_t size) override
		{
			_count = count;
			glBindBuffer(GetBufferTargetOpenGL(BufferTarget::vertex), _handle);
			glBufferData(GetBufferTargetOpenGL(BufferTarget::vertex), size, data, _usage);
			glBindBuffer(GetBufferTargetOpenGL(BufferTarget::vertex), 0);
		}
		void SetData(const std::vector<uint32_t>& data) override
		{
			SetData(data.data(), static_cast<uint32_t>(data.size()), data.size() * sizeof(uint32_t));
		}

		void UpdateData(const void* data, uint32_t size, uint32_t offset = 0) override
		{
			Bind();
			glBufferSubData(_target, offset, size, data);
			Unbind();
		}
		void UpdateData(const std::vector<uint32_t>& data) override
		{
			UpdateData(data.data(), data.size() * sizeof(uint32_t));
		}

		uint32_t GetCount() override
		{
			return _count;
		}

	private:
		GLenum _usage;
		GLenum _target;
		BufferDesc _desc;
		unsigned int _handle;

	};

	class VertexBufferOpenGL : public VertexBufferBase
	{
	public:
		VertexBufferOpenGL(const void* data, uint32_t size, BufferUsage usage)
		{
			_desc.usage = usage;
			_desc.target = BufferTarget::vertex;

			_usage = GetBufferUsageOpenGL(_desc.usage);
			_target = GetBufferTargetOpenGL(_desc.target);

			glGenBuffers(1, &_handle);

			Bind();
			glBufferData(_target, size, data, _usage);
			Unbind();
		}

		~VertexBufferOpenGL() override
		{
			if (_handle > 0)
			{
				glDeleteBuffers(1, &_handle);
			}
		}

		void Bind() override
		{
			glBindBuffer(_target, _handle);
		}
		void Unbind() override
		{
			glBindBuffer(_target, 0);
		}

		void SetData(const void* data, uint32_t size) override
		{
			Bind();
			glBufferData(_target, size, data, _usage);
			Unbind();
		}
		void SetData(const std::vector<glm::vec2>& data) override
		{
			SetData(data.data(), data.size() * sizeof(glm::vec2));
		}
		void SetData(const std::vector<glm::vec3>& data) override
		{
			SetData(data.data(), data.size() * sizeof(glm::vec3));
		}
		void SetData(const std::vector<glm::vec4>& data) override
		{
			SetData(data.data(), data.size() * sizeof(glm::vec4));
		}
		void SetData(const std::vector<glm::mat4>& data) override
		{
			SetData(data.data(), data.size() * sizeof(glm::mat4));
		}

		void UpdateData(const void* data, uint32_t size, uint32_t offset = 0) override
		{
			Bind();
			glBufferSubData(_target, offset, size, data);
			Unbind();
		}
		void UpdateData(const std::vector<glm::vec2>& data) override
		{
			UpdateData(data.data(), data.size() * sizeof(glm::vec2));
		}
		void UpdateData(const std::vector<glm::vec3>& data) override
		{
			UpdateData(data.data(), data.size() * sizeof(glm::vec3));
		}
		void UpdateData(const std::vector<glm::vec4>& data) override
		{
			UpdateData(data.data(), data.size() * sizeof(glm::vec4));
		}
		void UpdateData(const std::vector<glm::mat4>& data) override
		{
			UpdateData(data.data(), data.size() * sizeof(glm::mat4));
		}

		const BufferLayoutDesc& VertexBufferBase::GetLayout()
		{
			return _layoutDesc;
		}

		void VertexBufferBase::SetLayout(const BufferLayoutDesc& layout)
		{
			_layoutDesc = layout;
		}

		VertexBufferBase& AddElement(const BufferElementDesc& element) override
		{
			_layoutDesc.AddElement(element);
			return *this;
		}

	private:
		GLenum _usage;
		GLenum _target;
		BufferDesc _desc;
		unsigned int _handle;

	};
}
