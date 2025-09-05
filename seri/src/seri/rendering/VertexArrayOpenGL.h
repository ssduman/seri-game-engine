#pragma once

#include "seri/util/Util.h"
#include "seri/rendering/VertexArrayBase.h"

#include <glad/gl.h>

namespace seri
{
	GLenum GetShaderDataTypeOpenGL(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::int_type: return GL_INT;
			case ShaderDataType::int2_type: return GL_INT;
			case ShaderDataType::int3_type: return GL_INT;
			case ShaderDataType::int4_type: return GL_INT;
			case ShaderDataType::float_type: return GL_FLOAT;
			case ShaderDataType::float2_type: return GL_FLOAT;
			case ShaderDataType::float3_type: return GL_FLOAT;
			case ShaderDataType::float4_type: return GL_FLOAT;
			case ShaderDataType::mat3_type: return GL_FLOAT;
			case ShaderDataType::mat4_type: return GL_FLOAT;
		}

		return GL_FLOAT;
	};

	class VertexArrayOpenGL : public VertexArrayBase
	{
	public:
		VertexArrayOpenGL()
		{
			glCreateVertexArrays(1, &_handle);
		}

		~VertexArrayOpenGL() override
		{
			if (_handle)
			{
				glDeleteVertexArrays(1, &_handle);
			}
		}

		void Bind() override
		{
			glBindVertexArray(_handle);
		}

		void Unbind() override
		{
			glBindVertexArray(0);
		}

		void SetIndexBuffer(const std::shared_ptr<IndexBufferBase>& indexBuffer) override
		{
			glBindVertexArray(_handle);

			indexBuffer->Bind();

			this->indexBuffer = indexBuffer;
		}

		void AddVertexBuffer(const std::shared_ptr<VertexBufferBase>& vertexBuffer) override
		{
			Bind();
			vertexBuffer->Bind();

			auto& layout = vertexBuffer->GetLayout();
			for (auto& element : layout.GetElements())
			{
				switch (element.type)
				{
					case ShaderDataType::int_type:
					case ShaderDataType::int2_type:
					case ShaderDataType::int3_type:
					case ShaderDataType::int4_type:
						{
							int index = (int)element.loc;
							glEnableVertexAttribArray(index);
							glVertexAttribIPointer(
								index,
								element.GetCount(),
								GetShaderDataTypeOpenGL(element.type),
								layout.GetStride(),
								(const void*)element.GetOffset()
							);
						}
						break;
					case ShaderDataType::float_type:
					case ShaderDataType::float2_type:
					case ShaderDataType::float3_type:
					case ShaderDataType::float4_type:
						{
							int index = (int)element.loc;
							glEnableVertexAttribArray(index);
							glVertexAttribPointer(
								index,
								element.GetCount(),
								GetShaderDataTypeOpenGL(element.type),
								element.normalized ? GL_TRUE : GL_FALSE,
								layout.GetStride(),
								(const void*)element.GetOffset()
							);
						}
						break;
					case ShaderDataType::mat3_type:
					case ShaderDataType::mat4_type:
						{
							uint8_t count = element.GetCount();
							int index = (int)element.loc;
							for (uint8_t i = 0; i < count; i++)
							{
								glEnableVertexAttribArray(index);
								glVertexAttribPointer(
									index,
									count,
									GetShaderDataTypeOpenGL(element.type),
									element.normalized ? GL_TRUE : GL_FALSE,
									layout.GetStride(),
									(const void*)(element.GetOffset() + sizeof(float) * count * i));
								glVertexAttribDivisor(index, 1);
								index++;
							}
						}
						break;
				}
			}

			vertexBuffers.emplace_back(vertexBuffer);

			Unbind();
			vertexBuffer->Unbind();
		}

	private:
		unsigned int _handle;

	};
}
