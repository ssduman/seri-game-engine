#pragma once

#include "seri/renderer/AuxiliaryStructs.h"

#include <stdexcept>

namespace aux
{
	class AttributeBuilder
	{
	public:
		AttributeBuilder setIndex(Index index)
		{
			_attribute.index = toUInt(index);
			return *this;
		}

		AttributeBuilder setSize(GLint size)
		{
			_attribute.size = size;
			return *this;
		}

		AttributeBuilder setType(Type type)
		{
			_type = type;
			_attribute.type = toUInt(type);
			return *this;
		}

		AttributeBuilder setNormalized(bool normalized)
		{
			_attribute.normalized = normalized ? GL_TRUE : GL_FALSE;
			return *this;
		}

		AttributeBuilder setStride(GLsizei stride)
		{
			_attribute.stride = stride;
			return *this;
		}

		AttributeBuilder setPointer(const void* pointer)
		{
			_attribute.pointer = pointer;
			return *this;
		}

		AttributeBuilder& reset()
		{
			_type = Type::float_type;
			_attribute = Attribute{};
			return *this;
		}

		Attribute build()
		{
			auto attribute = _attribute;
			attribute.stride = static_cast<GLsizei>(calculateStride() * attribute.size);
			reset();
			return attribute;
		}

	private:
		unsigned long long calculateStride()
		{
			switch (_type)
			{
				case aux::Type::byte_type:
					return sizeof(GLbyte);
				case aux::Type::ubyte_type:
					return sizeof(GLubyte);
				case aux::Type::short_type:
					return sizeof(GLshort);
				case aux::Type::ushort_type:
					return sizeof(GLushort);
				case aux::Type::int_type:
					return sizeof(GLint);
				case aux::Type::uint_type:
					return sizeof(GLuint);
				case aux::Type::float_type:
					return sizeof(GLfloat);
				case aux::Type::double_type:
					return sizeof(GLdouble);
				default:
					throw std::runtime_error("type for opengl attribute could not find");
			}
		}

		Attribute _attribute;
		Type _type = aux::Type::float_type;

	};

	class DataBufferBuilder
	{
	public:
		DataBufferBuilder& setTarget(Target target)
		{
			_dataBuffer.target = toUInt(target);
			return *this;
		}

		DataBufferBuilder& setSize(GLsizeiptr size)
		{
			_dataBuffer.size = size;
			return *this;
		}

		DataBufferBuilder& setData(const void* data)
		{
			_dataBuffer.data = data;
			return *this;
		}

		DataBufferBuilder& setUsage(Usage usage)
		{
			_dataBuffer.usage = toUInt(usage);
			return *this;
		}

		DataBufferBuilder& reset()
		{
			_dataBuffer = DataBuffer{};
			return *this;
		}

		DataBuffer build()
		{
			auto dataBuffer = _dataBuffer;
			reset();
			return dataBuffer;
		}

	private:
		DataBuffer _dataBuffer;

	};

	class SubDataBufferBuilder
	{
	public:
		SubDataBufferBuilder& setTarget(Target target)
		{
			_subDataBuffer.target = toUInt(target);
			return *this;
		}

		SubDataBufferBuilder& setOffset(GLintptr offset)
		{
			_subDataBuffer.offset = offset;
			return *this;
		}

		SubDataBufferBuilder& setSize(GLsizeiptr size)
		{
			_subDataBuffer.size = size;
			return *this;
		}

		SubDataBufferBuilder& setData(const void* data)
		{
			_subDataBuffer.data = data;
			return *this;
		}

		SubDataBufferBuilder& reset()
		{
			_subDataBuffer = SubDataBuffer{};
			return *this;
		}

		SubDataBuffer build()
		{
			auto subDataBuffer = _subDataBuffer;
			reset();
			return subDataBuffer;
		}

	private:
		SubDataBuffer _subDataBuffer;

	};

	class MapBufferBuilder
	{
	public:
		MapBufferBuilder& setTarget(Target target)
		{
			_mapBuffer.target = toUInt(target);
			return *this;
		}

		MapBufferBuilder& setAccess(Access access)
		{
			_mapBuffer.access = toUInt(access);
			return *this;
		}

		MapBufferBuilder& setSize(size_t size)
		{
			_mapBuffer.size = size;
			return *this;
		}

		MapBufferBuilder& setData(const void* data)
		{
			_mapBuffer.data = data;
			return *this;
		}

		MapBufferBuilder& reset()
		{
			_mapBuffer = MapBuffer{};
			return *this;
		}

		MapBuffer build()
		{
			auto mapBuffer = _mapBuffer;
			reset();
			return mapBuffer;
		}

	private:
		MapBuffer _mapBuffer;

	};
}
