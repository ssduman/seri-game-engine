#pragma once

#include "seri/util/Util.h"
#include "seri/rendering/render/RenderingUtil.h"

namespace seri
{
	enum class BufferTarget
	{
		vertex,
		index,
		uniform,
	};

	enum class BufferUsage
	{
		stream_draw,
		static_draw,
		dynamic_draw,
	};

	enum class UniformBinding
	{
		camera,
		light,
	};

	struct BufferDesc
	{
		BufferUsage usage;
		BufferTarget target;
	};

	class IndexBufferBase
	{
	public:
		virtual ~IndexBufferBase() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void SetData(const void* data, uint32_t count, uint32_t size) = 0;
		virtual void SetData(const std::vector<uint32_t>& data) = 0;

		virtual void UpdateData(const void* data, uint32_t size, uint32_t offset = 0) = 0;
		virtual void UpdateData(const std::vector<uint32_t>& data) = 0;

		virtual uint32_t GetCount() = 0;

		static std::shared_ptr<IndexBufferBase> Create(const uint32_t* data, uint32_t count);
		static std::shared_ptr<IndexBufferBase> Create(const std::vector<uint32_t>& indices);

	protected:
		uint32_t _count;

	};

	class VertexBufferBase
	{
	public:
		virtual ~VertexBufferBase() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void SetData(const void* data, uint32_t size) = 0;
		virtual void SetData(const std::vector<glm::vec2>& data) = 0;
		virtual void SetData(const std::vector<glm::vec3>& data) = 0;
		virtual void SetData(const std::vector<glm::vec4>& data) = 0;
		virtual void SetData(const std::vector<glm::mat4>& data) = 0;

		virtual void UpdateData(const void* data, uint32_t size, uint32_t offset = 0) = 0;
		virtual void UpdateData(const std::vector<glm::vec2>& data) = 0;
		virtual void UpdateData(const std::vector<glm::vec3>& data) = 0;
		virtual void UpdateData(const std::vector<glm::vec4>& data) = 0;
		virtual void UpdateData(const std::vector<glm::mat4>& data) = 0;

		virtual const BufferLayoutDesc& GetLayout() { return _layoutDesc; }
		virtual void SetLayout(const BufferLayoutDesc& layout) { _layoutDesc = layout; }

		virtual VertexBufferBase& AddElement(const BufferElementDesc& element) = 0;
		virtual void AddElements(std::initializer_list<BufferElementDesc> elements) = 0;

		static std::shared_ptr<VertexBufferBase> Create(const void* data, uint32_t size, BufferUsage usage = BufferUsage::static_draw);
		static std::shared_ptr<VertexBufferBase> Create(const std::vector<glm::vec2>& data, BufferUsage usage = BufferUsage::static_draw);
		static std::shared_ptr<VertexBufferBase> Create(const std::vector<glm::vec3>& data, BufferUsage usage = BufferUsage::static_draw);
		static std::shared_ptr<VertexBufferBase> Create(const std::vector<glm::vec4>& data, BufferUsage usage = BufferUsage::static_draw);
		static std::shared_ptr<VertexBufferBase> Create(const std::vector<glm::mat4>& data, BufferUsage usage = BufferUsage::static_draw);

	protected:
		BufferLayoutDesc _layoutDesc{};

	};

	class UniformBufferBase
	{
	public:
		virtual ~UniformBufferBase() = default;

		virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) = 0;

		static std::shared_ptr<UniformBufferBase> Create(uint32_t size, UniformBinding binding);

	};
}
