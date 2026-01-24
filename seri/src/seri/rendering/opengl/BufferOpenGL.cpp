#include "Seripch.h"

#include "seri/rendering/opengl/BufferOpenGL.h"

namespace seri
{
	GLenum GetBufferTargetOpenGL(BufferTarget target)
	{
		switch (target)
		{
			case BufferTarget::vertex: return GL_ARRAY_BUFFER;
			case BufferTarget::index: return GL_ELEMENT_ARRAY_BUFFER;
			case BufferTarget::uniform: return GL_UNIFORM_BUFFER;
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
	}

	IndexBufferOpenGL::IndexBufferOpenGL(const uint32_t* data, uint32_t count)
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

	IndexBufferOpenGL::~IndexBufferOpenGL()
	{
		if (_handle > 0)
		{
			glDeleteBuffers(1, &_handle);
		}
	}

	void IndexBufferOpenGL::Bind()
	{
		glBindBuffer(_target, _handle);
	}

	void IndexBufferOpenGL::Unbind()
	{
		glBindBuffer(_target, 0);
	}

	void IndexBufferOpenGL::SetData(const void* data, uint32_t count, uint32_t size)
	{
		_count = count;
		glBindBuffer(GetBufferTargetOpenGL(BufferTarget::vertex), _handle);
		glBufferData(GetBufferTargetOpenGL(BufferTarget::vertex), size, data, _usage);
		glBindBuffer(GetBufferTargetOpenGL(BufferTarget::vertex), 0);
	}

	void IndexBufferOpenGL::SetData(const std::vector<uint32_t>& data)
	{
		SetData(data.data(), static_cast<uint32_t>(data.size()), data.size() * sizeof(uint32_t));
	}

	void IndexBufferOpenGL::UpdateData(const void* data, uint32_t size, uint32_t offset)
	{
		Bind();
		glBufferSubData(_target, offset, size, data);
		Unbind();
	}

	void IndexBufferOpenGL::UpdateData(const std::vector<uint32_t>& data)
	{
		UpdateData(data.data(), data.size() * sizeof(uint32_t));
	}

	uint32_t IndexBufferOpenGL::GetCount()
	{
		return _count;
	}

	VertexBufferOpenGL::VertexBufferOpenGL(const void* data, uint32_t size, BufferUsage usage)
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

	VertexBufferOpenGL::~VertexBufferOpenGL()
	{
		if (_handle > 0)
		{
			glDeleteBuffers(1, &_handle);
		}
	}

	void VertexBufferOpenGL::Bind()
	{
		glBindBuffer(_target, _handle);
	}

	void VertexBufferOpenGL::Unbind()
	{
		glBindBuffer(_target, 0);
	}

	void VertexBufferOpenGL::SetData(const void* data, uint32_t size)
	{
		Bind();
		glBufferData(_target, size, data, _usage);
		Unbind();
	}

	void VertexBufferOpenGL::SetData(const std::vector<glm::mat4>& data)
	{
		SetData(data.data(), data.size() * sizeof(glm::mat4));
	}

	void VertexBufferOpenGL::UpdateData(const void* data, uint32_t size, uint32_t offset)
	{
		Bind();
		glBufferSubData(_target, offset, size, data);
		Unbind();
	}

	void VertexBufferOpenGL::UpdateData(const std::vector<glm::mat4>& data)
	{
		UpdateData(data.data(), data.size() * sizeof(glm::mat4));
	}

	VertexBufferBase& VertexBufferOpenGL::AddElement(const BufferElementDesc& element)
	{
		_layoutDesc.AddElement(element);
		return *this;
	}

	void VertexBufferOpenGL::AddElements(std::initializer_list<BufferElementDesc> elements)
	{
		for (const auto& element : elements)
		{
			_layoutDesc.AddElement(element);
		}
	}

	void VertexBufferOpenGL::UpdateData(const std::vector<glm::vec4>& data)
	{
		UpdateData(data.data(), data.size() * sizeof(glm::vec4));
	}

	void VertexBufferOpenGL::UpdateData(const std::vector<glm::vec3>& data)
	{
		UpdateData(data.data(), data.size() * sizeof(glm::vec3));
	}

	void VertexBufferOpenGL::UpdateData(const std::vector<glm::vec2>& data)
	{
		UpdateData(data.data(), data.size() * sizeof(glm::vec2));
	}

	void VertexBufferOpenGL::SetData(const std::vector<glm::vec4>& data)
	{
		SetData(data.data(), data.size() * sizeof(glm::vec4));
	}

	void VertexBufferOpenGL::SetData(const std::vector<glm::vec3>& data)
	{
		SetData(data.data(), data.size() * sizeof(glm::vec3));
	}

	void VertexBufferOpenGL::SetData(const std::vector<glm::vec2>& data)
	{
		SetData(data.data(), data.size() * sizeof(glm::vec2));
	}

	UniformBufferOpenGL::UniformBufferOpenGL(uint32_t size, UniformBinding binding)
	{
		glCreateBuffers(1, &_handle);
		glNamedBufferData(_handle, size, nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, static_cast<uint32_t>(binding), _handle);
	}

	UniformBufferOpenGL::~UniformBufferOpenGL()
	{
		glDeleteBuffers(1, &_handle);
	}

	void UniformBufferOpenGL::SetData(const void* data, uint32_t size, uint32_t offset)
	{
		glNamedBufferSubData(_handle, offset, size, data);
	}

}
