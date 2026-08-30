#pragma once

#include "seri/util/Util.h"
#include "seri/rendering/common/BufferBase.h"

#include <glad/gl.h>

namespace seri
{
	GLenum GetBufferTargetOpenGL(BufferTarget target);

	GLenum GetBufferUsageOpenGL(BufferUsage usage);

	class IndexBufferOpenGL : public IndexBufferBase
	{
	public:
		IndexBufferOpenGL(const uint32_t* data, uint32_t count);
		~IndexBufferOpenGL() override;

		void Bind() override;
		void Unbind() override;

		void SetData(const void* data, uint32_t count, uint32_t size) override;
		void SetData(const std::vector<uint32_t>& data) override;

		void UpdateData(const void* data, uint32_t size, uint32_t offset = 0) override;
		void UpdateData(const std::vector<uint32_t>& data) override;

		uint32_t GetCount() override;

	private:
		GLenum _usage;
		GLenum _target;
		BufferDesc _desc;
		unsigned int _handle;

	};

	class VertexBufferOpenGL : public VertexBufferBase
	{
	public:
		VertexBufferOpenGL(const void* data, uint32_t size, BufferUsage usage);
		~VertexBufferOpenGL() override;

		void Bind() override;
		void Unbind() override;

		void SetData(const void* data, uint32_t size) override;
		void SetData(const std::vector<glm::vec2>& data) override;
		void SetData(const std::vector<glm::vec3>& data) override;
		void SetData(const std::vector<glm::vec4>& data) override;
		void SetData(const std::vector<glm::mat4>& data) override;

		void UpdateData(const void* data, uint32_t size, uint32_t offset = 0) override;
		void UpdateData(const std::vector<glm::vec2>& data) override;
		void UpdateData(const std::vector<glm::vec3>& data) override;
		void UpdateData(const std::vector<glm::vec4>& data) override;
		void UpdateData(const std::vector<glm::mat4>& data) override;

		VertexBufferBase& AddElement(const BufferElementDesc& element) override;
		void AddElements(std::initializer_list<BufferElementDesc> elements) override;

	private:
		GLenum _usage;
		GLenum _target;
		BufferDesc _desc;
		unsigned int _handle;

	};

	class UniformBufferOpenGL : public UniformBufferBase
	{
	public:
		UniformBufferOpenGL(uint32_t size, UniformBinding binding);
		~UniformBufferOpenGL() override;

		virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) override;

	private:
		unsigned int _handle;

	};
}
